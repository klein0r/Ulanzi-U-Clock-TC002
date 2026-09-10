# TC002 Year Progress Bar

> **New here?** See [QUICKSTART.md](QUICKSTART.md)

## Introduction

Shows this year's progress on the TC002 (U-Clock) as a 52-column × 7-row calendar dot matrix.

- Days already past: dark green
- Today: bright green breathing animation
- Top right corner: progress percentage

**Author**: 王行知

## Preview

See `preview/demo.gif`

## Dependencies

- Python 3 + Pillow (`pip install pillow`)
- mosquitto_pub (`brew install mosquitto`)
- An MQTT broker (reachable from both the TC002 and your computer)

## Installation

1. Add the following to `~/.claude/settings.json`:

   ```json
   {
     "statusLine": {
       "type": "command",
       "command": "node /path/to/lab/claude_statusline_bridge.js"
     }
   }
   ```

2. Add the environment variables to `~/.zshrc`:

   ```bash
   export TC002_MQTT_HOST=<your MQTT broker address>
   export TC002_MQTT_TOPIC=ulanzi_1bf6/custom/year_progress
   ```

3. Restart Claude Code; the TC002 display is then updated automatically after every response.

## Manual Testing

```bash
cd apps/mqtt/year-progress-bar

# Generate and publish:
TC002_MQTT_HOST=<broker address> python3 lab/render_year_progress.py | xargs -I{} mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m '{"duration":31536000,"text":[],"image":[{"data":"data:image/gif;base64,{}","position":[0,0]}],"draw":[]}'

# Test with a specific date:
python3 lab/render_year_progress.py --date 2026-06-25 --output preview/demo.gif
```

## MQTT Payload

```json
{
  "duration": 31536000,
  "text": [],
  "image": [{"data": "data:image/gif;base64,...", "position": [0, 0]}],
  "draw": []
}
```

## Known Issues

- After receiving a Custom App update, the TC002 does not necessarily switch to that app automatically.
- The progress bar updates automatically once a day (triggered on a schedule by the blueprint).

## License

GPL-3.0-or-later.
