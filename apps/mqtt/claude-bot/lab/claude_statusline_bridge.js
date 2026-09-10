#!/usr/bin/env node
/**
 * TC002 Claude Bot - statusLine bridge script
 *
 * Receives JSON from the Claude Code statusLine hook, extracts the 5-hour/7-day quota usage,
 * publishes it to the TC002 over MQTT and writes the state to a temp file for other tools.
 *
 * Usage:
 *   # As a Claude Code statusLine hook (configured in ~/.claude/settings.json):
 *   #   "statusLine": { "type": "command", "command": "node path/to/claude_statusline_bridge.js" }
 *   #
 *   # Or to test manually:
 *   echo '{"rate_limits":{"five_hour":{"used_percentage":75},"seven_day":{"used_percentage":42}}}' | node lab/claude_statusline_bridge.js
 *
 * Environment variables:
 *   TC002_MQTT_HOST    MQTT broker address (default: 127.0.0.1)
 *   TC002_MQTT_PORT    MQTT broker port (default: 1883)
 *   TC002_MQTT_TOPIC   Custom App topic (default: ulanzi_1bf6/custom/claude_bot)
 *   TC002_DURATION     payload display duration in seconds (default: 31536000, i.e. one year, stays on)
 *   TC002_STATE_FILE   path of the state file (default: /tmp/claude-statusline-state.json)
 */

const { execFileSync } = require("node:child_process");
const fs = require("node:fs");
const path = require("node:path");

// Configuration, read from environment variables with defaults
const MQTT_HOST = process.env.TC002_MQTT_HOST || "127.0.0.1";
const MQTT_PORT = process.env.TC002_MQTT_PORT || "1883";
const MQTT_TOPIC = process.env.TC002_MQTT_TOPIC || "ulanzi_1bf6/custom/claude_bot";
const DURATION = parseInt(process.env.TC002_DURATION || "31536000", 10); // one year by default, stays on
const STATE_FILE = process.env.TC002_STATE_FILE || "/tmp/claude-statusline-state.json";
const RENDER_SCRIPT = path.join(__dirname, "render_usage.py");

// Read the JSON data sent by Claude Code from stdin
function parseInput() {
  const buf = fs.readFileSync(0, "utf8"); // fd 0 = stdin
  return JSON.parse(buf.trim());
}

// Extract the 5-hour and 7-day quota usage percentages
function extractRateLimits(data) {
  const rl = data.rate_limits || {};
  const fiveHour = rl.five_hour || {};
  const sevenDay = rl.seven_day || {};
  return {
    five_hour_pct: Math.max(0, Math.min(100, parseInt(fiveHour.used_percentage) || 0)),
    seven_day_pct: Math.max(0, Math.min(100, parseInt(sevenDay.used_percentage) || 0)),
  };
}

// Extract session information (model, cost, duration, etc.)
function extractSessionInfo(data) {
  return {
    model: (data.model && data.model.display_name) || "unknown",
    cost_usd: (data.cost && data.cost.total_cost_usd) || 0,
    duration_ms: (data.cost && data.cost.total_duration_ms) || 0,
    ctx_pct: (data.context_window && data.context_window.used_percentage) || 0,
    lines_added: (data.cost && data.cost.total_lines_added) || 0,
    lines_removed: (data.cost && data.cost.total_lines_removed) || 0,
  };
}

// Write the state to a file (read by scripts such as publish_usage.sh)
function writeState(state) {
  try {
    fs.writeFileSync(STATE_FILE, JSON.stringify(state, null, 2));
  } catch (e) {
    // Failing to write the state file must not affect the main flow
  }
}

// Call the Python script to render the 52x16 GIF
function renderGif(fiveHourPct, sevenDayPct) {
  try {
    const b64 = execFileSync("python3", [
      RENDER_SCRIPT,
      String(fiveHourPct),
      String(sevenDayPct),
    ], { encoding: "utf8", stdio: ["ignore", "pipe", "pipe"] }).trim();
    return b64;
  } catch (e) {
    console.error(`[bridge] rendering failed: ${e.message}`);
    return null;
  }
}

// Publish to the TC002 over MQTT
function publishMqtt(b64) {
  const payload = JSON.stringify({
    duration: DURATION,
    text: [],
    image: [{ data: `data:image/gif;base64,${b64}`, position: [0, 0] }],
    draw: [],
  });
  try {
    execFileSync("mosquitto_pub", [
      "-h", MQTT_HOST,
      "-p", MQTT_PORT,
      "-t", MQTT_TOPIC,
      "-m", payload,
    ], { stdio: ["ignore", "ignore", "pipe"] });
    return true;
  } catch (e) {
    console.error(`[bridge] MQTT publish failed: ${e.message}`);
    return false;
  }
}

// Main flow
function main() {
  const data = parseInput();
  const limits = extractRateLimits(data);
  const session = extractSessionInfo(data);

  // Write the state file
  const state = {
    timestamp: new Date().toISOString(),
    rate_limits: limits,
    session,
  };
  writeState(state);

  // Render the GIF
  const b64 = renderGif(limits.five_hour_pct, limits.seven_day_pct);
  if (!b64) {
    process.exit(1);
  }

  // Publish over MQTT
  const published = publishMqtt(b64);

  // Print the status line for Claude Code to display (terminal status bar)
  const pct = limits.five_hour_pct;
  const color = pct >= 90 ? "\x1b[31m" : pct >= 70 ? "\x1b[33m" : "\x1b[32m";
  const reset = "\x1b[0m";
  process.stdout.write(
    `◆ ${session.model} │ 5H:${color}${limits.five_hour_pct}%${reset} 7d:${color}${limits.seven_day_pct}%${reset} │ $${session.cost_usd.toFixed(2)}\n`
  );
}

main();
