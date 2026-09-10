#!/usr/bin/env bash
# ─────────────────────────────────────────────────────────────
# TC002 Claude Bot - quota usage publishing script
# ─────────────────────────────────────────────────────────────
# Reads the quota percentages from the Claude Code statusLine state file or from arguments,
# renders the 52x16 quota usage GIF and publishes it to the TC002 Custom App over MQTT.
#
# Two modes:
#   1. read from the state file (written by claude_statusline_bridge.js)
#   2. pass the two percentages as arguments
#
# Dependencies:
#   - Python 3 + Pillow (pip install pillow)
#   - mosquitto_pub (brew install mosquitto)
#
# Environment variables:
#   TC002_MQTT_HOST                MQTT broker address (default: 127.0.0.1)
#   TC002_MQTT_PORT                MQTT broker port (default: 1883)
#   TC002_MQTT_TOPIC               Custom App topic (default: ulanzi_1bf6/custom/claude_bot)
#   TC002_DURATION                 payload display duration in seconds (default: 86400)
#   TC002_STATE_FILE               path of the state file (default: /tmp/claude-statusline-state.json)
#
# Usage:
#   # read from the state file (recommended):
#   TC002_MQTT_HOST=10.19.1.58 bash lab/publish_usage.sh
#
#   # pass the percentages manually:
#   bash lab/publish_usage.sh 75 42
#
#   # polling mode (re-reads the state file every 300 seconds):
#   bash lab/publish_usage.sh --loop 300
# ─────────────────────────────────────────────────────────────

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# -- Configuration ------------------------------------------
MQTT_HOST="${TC002_MQTT_HOST:-127.0.0.1}"
MQTT_PORT="${TC002_MQTT_PORT:-1883}"
MQTT_TOPIC="${TC002_MQTT_TOPIC:-ulanzi_1bf6/custom/claude_bot}"
DURATION="${TC002_DURATION:-31536000}"  # one year by default, stays on
STATE_FILE="${TC002_STATE_FILE:-/tmp/claude-statusline-state.json}"

# -- Help text ----------------------------------------------
if [[ "${1:-}" == "--help" || "${1:-}" == "-h" ]]; then
  cat <<EOF
Usage: bash lab/publish_usage.sh [5-hour-quota-pct 7-day-quota-pct] [--loop SECONDS]

Publishes the Claude Code quota usage to the TC002 over MQTT.

With no arguments: read from the state file (written by claude_statusline_bridge.js).
With two numbers: use them directly as the 5-hour and 7-day quota percentages.

Optional environment variables:
  TC002_MQTT_HOST    (default: 127.0.0.1)
  TC002_MQTT_PORT    (default: 1883)
  TC002_MQTT_TOPIC   (default: ulanzi_1bf6/custom/claude_bot)
  TC002_DURATION     (default: 86400)
  TC002_STATE_FILE   (default: /tmp/claude-statusline-state.json)

Options:
  --loop SECONDS     keep running, publishing every SECONDS seconds
EOF
  exit 0
fi

# -- Dependency check ---------------------------------------
for cmd in python3 mosquitto_pub; do
  if ! command -v "$cmd" &>/dev/null; then
    echo "[x] missing dependency: $cmd" >&2
    exit 1
  fi
done

# -- Read the quota percentages from the state file ---------
read_from_state_file() {
  if [[ ! -f "$STATE_FILE" ]]; then
    echo "[!] state file does not exist: $STATE_FILE" >&2
    echo "    run claude_statusline_bridge.js first, or pass the percentages manually." >&2
    return 1
  fi
  python3 -c "
import sys, json
with open('$STATE_FILE') as f:
    state = json.load(f)
rl = state.get('rate_limits', {})
print(rl.get('five_hour_pct', 0))
print(rl.get('seven_day_pct', 0))
"
}

# -- Render the quota usage GIF -----------------------------
render_gif() {
  local five_hour_pct="$1"
  local seven_day_pct="$2"
  echo "[*] rendering quota usage GIF (5H:${five_hour_pct}% 7d:${seven_day_pct}%)..." >&2
  python3 "$SCRIPT_DIR/render_usage.py" "$five_hour_pct" "$seven_day_pct"
}

# -- Publish over MQTT --------------------------------------
publish_mqtt() {
  local b64="$1"
  local payload
  payload=$(cat <<EOP
{"duration":$DURATION,"text":[],"image":[{"data":"data:image/gif;base64,$b64","position":[0,0]}],"draw":[]}
EOP
  )
  echo "[*] publishing to $MQTT_TOPIC..." >&2
  mosquitto_pub -h "$MQTT_HOST" -p "$MQTT_PORT" -t "$MQTT_TOPIC" -m "$payload"
  echo "[ok] published to $MQTT_TOPIC" >&2
}

# -- Main flow ----------------------------------------------
run_once_manual() {
  local five_hour_pct="$1"
  local seven_day_pct="$2"
  local b64
  b64=$(render_gif "$five_hour_pct" "$seven_day_pct")
  publish_mqtt "$b64"
}

run_once_from_state() {
  local limits
  limits=$(read_from_state_file) || return 1
  local five_hour_pct seven_day_pct
  five_hour_pct=$(echo "$limits" | head -1)
  seven_day_pct=$(echo "$limits" | tail -1)
  echo "[*] quota usage: 5H=${five_hour_pct}% 7d=${seven_day_pct}%" >&2
  local b64
  b64=$(render_gif "$five_hour_pct" "$seven_day_pct")
  publish_mqtt "$b64"
}

# -- Entry point --------------------------------------------
if [[ $# -ge 2 && "$1" != "--loop" ]]; then
  run_once_manual "$1" "$2"
  shift 2
else
  run_once_from_state
fi

# Optional polling mode
if [[ "${1:-}" == "--loop" ]]; then
  LOOP_SECONDS="${2:-300}"
  echo "[*] polling every ${LOOP_SECONDS}s (Ctrl+C to stop)..." >&2
  while true; do
    sleep "$LOOP_SECONDS"
    run_once_from_state || echo "[!] this round failed, retrying next round." >&2
  done
fi
