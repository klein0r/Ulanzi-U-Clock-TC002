#!/usr/bin/env node
/**
 * DEPRECATED: this script has been superseded by claude_statusline_bridge.js.
 *
 * TC002 Claude Bot - Codex usage reader (legacy).
 *
 * Reads the daily Codex usage via `ccusage codex daily --json` and
 * computes today's spend (USD) and this week's spend (USD).
 * Cache-read tokens are excluded, because prompt caching is not billed.
 *
 * No budget configuration needed - the cost comes straight from ccusage, which applies per-model pricing to non-cached tokens.
 *
 * Usage:
 *   node lab/claude_usage_snapshot.js
 *
 * Output:
 *   JSON with today's and this week's Codex spend (USD) and the actual token counts.
 */

const { execFileSync } = require("node:child_process");

const TIMEZONE = process.env.TC002_TIMEZONE || "Asia/Shanghai";

function usage() {
  console.log(`Usage:
  node lab/claude_usage_snapshot.js

Environment:
  TC002_TIMEZONE    timezone for date grouping (default: Asia/Shanghai)
`);
}

function runCcusage() {
  const output = execFileSync(
    "npx",
    ["--yes", "ccusage", "codex", "daily", "--json", "--timezone", TIMEZONE, "--offline"],
    { encoding: "utf8", stdio: ["ignore", "pipe", "pipe"] },
  );
  return JSON.parse(output);
}

function todayDateStr() {
  const now = new Date();
  const formatter = new Intl.DateTimeFormat("en-CA", {
    timeZone: TIMEZONE,
    year: "numeric",
    month: "2-digit",
    day: "2-digit",
  });
  return formatter.format(now);
}

function weekStartDateStr() {
  const now = new Date();
  const parts = new Intl.DateTimeFormat("en-CA", {
    timeZone: TIMEZONE,
    weekday: "short",
  }).formatToParts(now);
  const shortWeekday = parts.find((p) => p.type === "weekday")?.value || "";
  const dowMap = { Sun: 0, Mon: 1, Tue: 2, Wed: 3, Thu: 4, Fri: 5, Sat: 6 };
  const dow = dowMap[shortWeekday] ?? 1;
  const daysSinceMonday = dow === 0 ? 6 : dow - 1;

  const monday = new Date(now);
  monday.setDate(monday.getDate() - daysSinceMonday);
  const mondayFormatter = new Intl.DateTimeFormat("en-CA", {
    timeZone: TIMEZONE,
    year: "numeric",
    month: "2-digit",
    day: "2-digit",
  });
  return mondayFormatter.format(monday);
}

function actualTokens(day) {
  return (
    (day.inputTokens || 0) +
    (day.outputTokens || 0) +
    (day.reasoningOutputTokens || 0)
  );
}

if (process.argv.includes("--help") || process.argv.includes("-h")) {
  usage();
  process.exit(0);
}

const report = runCcusage();
const todayStr = todayDateStr();
const weekStart = weekStartDateStr();

let todayActualTokens = 0;
let todayCost = 0;
let weekActualTokens = 0;
let weekCost = 0;

for (const day of report.daily || []) {
  const date = day.date || "";

  if (date === todayStr) {
    todayActualTokens += actualTokens(day);
    todayCost += day.costUSD || 0;
  }
  if (date >= weekStart) {
    weekActualTokens += actualTokens(day);
    weekCost += day.costUSD || 0;
  }
}

const result = {
  source: "ccusage codex daily",
  timezone: TIMEZONE,
  note: "Costs are calculated from non-cache tokens using per-model pricing. Cache-read tokens are excluded.",
  today: {
    date: todayStr,
    actualTokens: todayActualTokens,
    costUSD: Number(todayCost.toFixed(2)),
  },
  week: {
    start: weekStart,
    end: todayStr,
    actualTokens: weekActualTokens,
    costUSD: Number(weekCost.toFixed(2)),
  },
};

console.log(JSON.stringify(result, null, 2));
