# Local Testing Tools

This directory contains scripts for local testing and for hooking up real usage data, intended for development and debugging.

> **Full installation guide**: see [docs/QUICKSTART.md](../docs/QUICKSTART.md)

## Hooking Up Real Claude Code Usage

The scripts in this directory implement the complete real-time rate limit display, and **only ever show real data**:

```text
Claude Code statusLine hook
  → claude_statusline_bridge.js (receives JSON on stdin, extracts the rate limit percentages)
  → render_usage.py (renders the 52x16 rate limit usage GIF)
  → mosquitto_pub (publishes to the TC002 over MQTT)
```

### Quick Test

```bash
cd apps/mqtt/claude-bot

# Read from the state file and publish:
TC002_MQTT_HOST=<your broker address> bash lab/publish_usage.sh

# Render the GIF to disk:
python3 lab/render_usage.py 50 30 --file /tmp/claude_bot_usage.gif
```

### Live Mode (statusLine hook)

1. Configure the Claude Code statusLine hook (see `docs/USAGE_INTEGRATION.md` for details):

   ```json
   {
     "statusLine": {
       "type": "command",
       "command": "node /path/to/lab/claude_statusline_bridge.js"
     }
   }
   ```

2. The bridge script runs automatically after every Claude Code response:
   - Extracts `rate_limits.five_hour.used_percentage` and `seven_day.used_percentage` from stdin
   - Writes the state to `/tmp/claude-statusline-state.json`
   - Renders the 52x16 GIF and publishes it to the TC002 over MQTT

3. Polling mode (reads the state file every 300 seconds):

   ```bash
   bash lab/publish_usage.sh --loop 300
   ```

### Environment Variables

| Variable | Required | Default | Description |
|---|---|---|---|
| `TC002_MQTT_HOST` | No | `127.0.0.1` | MQTT broker address |
| `TC002_MQTT_PORT` | No | `1883` | MQTT broker port |
| `TC002_MQTT_TOPIC` | No | `ulanzi_1bf6/custom/claude_bot` | Custom App topic |
| `TC002_DURATION` | No | `31536000` | Display duration (seconds), one year by default, so it stays on permanently |
| `TC002_STATE_FILE` | No | `/tmp/claude-statusline-state.json` | Path of the state file |

### How the Flow Works

| Step | Script | Function |
|---|---|---|
| 1. Hook | `claude_statusline_bridge.js` | Receives the Claude Code statusLine JSON, extracts the rate limit percentages, writes the state file, renders the GIF and publishes over MQTT |
| 1a. Render | `render_usage.py` | Takes two percentages (0-100), generates a 52×16 animated GIF, outputs base64 |
| 1b. Publish | `mosquitto_pub` | Wraps the base64 GIF into a TC002 Custom App JSON payload and publishes it to the MQTT topic |
| 2. State | `/tmp/claude-statusline-state.json` | The bridge writes the rate limit data here; publish_usage.sh can read it |

### Usage Bar Color Coding

| Percentage | Color | Meaning |
|---|---|---|
| < 70% | Green | Normal |
| 70–90% | Yellow | Caution |
| > 90% | Red | Danger |

### render_usage.py — Standalone Renderer

```bash
# Print base64 to stdout:
python3 lab/render_usage.py 50 30

# Also write a GIF file:
python3 lab/render_usage.py 50 30 --file /tmp/claude_bot_usage.gif
```

Requires Python 3 + Pillow (`pip install pillow`).

### Legacy Script (Deprecated)

`claude_usage_snapshot.js` is the legacy script, which reads the Codex spending history via `ccusage`. It has been superseded by the statusLine bridge.

## Why Several Scripts

- `claude_statusline_bridge.js` — the Claude Code hook: reads stdin, writes the state file and publishes over MQTT
- `render_usage.py` — a pure renderer that other tools can reuse
- `publish_usage.sh` — a lightweight orchestrator that reads the state file and publishes over MQTT
