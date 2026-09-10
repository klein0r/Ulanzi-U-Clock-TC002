# TC002 Claude Bot

> **New here?** Start with the [Quick Start Guide](QUICKSTART.md) and get the TC002 showing your real remaining Claude Code usage in 5 minutes.

## Introduction

As everyone knows, when 100% of your 5H and 7D quota is still left, you feel like Homelander freshly injected with V1, your combat power off the charts; and when your Claude Code quota runs out, even Homelander can only kneel and beg Butcher for mercy ("Plz! More Token!"). This app turns the Ulanzi U-Clock TC002 (a.k.a. U-Clock/Pixbar) into a Claude Code companion: on the right, the Claude Bot mascot keeps you company while vibe coding, and on the left it **shows your remaining Claude Code 5H/7D quota in real time**.

**Author**: 王行知

### What Is Displayed

| Label | Meaning |
|---|---|
| `5H` | Usage of the 5-hour window quota (%) |
| `7d` | Usage of the 7-day window quota (%) |

Color thresholds:

| Range | Color | Meaning |
|---|---|---|
| < 70% | Green | Normal |
| 70–90% | Yellow | Caution |
| > 90% | Red | Danger |

## Preview

See `preview/demo.gif` (looping animation) and `preview/demo_usage.gif` (quota display).

## Dependencies

- Python 3 + Pillow (`pip install pillow`) — for rendering the GIF
- mosquitto_pub (`brew install mosquitto`) — for publishing over MQTT
- An MQTT broker (reachable from both the TC002 and your computer)
- A TC002 device connected to Wi-Fi and configured for MQTT

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
   export TC002_MQTT_TOPIC=<your device topic>
   ```

3. Restart Claude Code; the TC002 display is then updated automatically after every response.

> **Full installation steps**: see [QUICKSTART.md](QUICKSTART.md) or [USAGE_INTEGRATION.md](USAGE_INTEGRATION.md)

## Hooking Up Real Usage Data

### Data Flow

```text
Claude Code statusLine hook
  → claude_statusline_bridge.js (extracts the 5h/7d quota percentages)
  → render_usage.py (renders the 52×16 GIF)
  → mosquitto_pub (publishes over MQTT)
  → the TC002 shows Claude Bot + the real quota usage
```

### Manual Testing

```bash
cd apps/mqtt/claude-bot

# Read from the state file and publish:
TC002_MQTT_HOST=<your broker address> bash lab/publish_usage.sh
```

### The Scripts

| Script | Responsibility |
|---|---|
| `lab/claude_statusline_bridge.js` | Receives the statusLine JSON, extracts the quotas, renders and publishes |
| `lab/render_usage.py` | Takes the percentages and renders a base64 GIF |
| `lab/publish_usage.sh` | Reads the state file and publishes over MQTT |

## How It Works

```text
Claude Code statusLine hook
  → claude_statusline_bridge.js
    → extracts rate_limits.five_hour.used_percentage
    → extracts rate_limits.seven_day.used_percentage
    → render_usage.py renders the 52×16 GIF
    → mosquitto_pub publishes it to the TC002
  → the TC002 shows Claude Bot + the real quota usage
```

## Configuration Options

| Option | Description |
|---|---|
| `TC002_MQTT_HOST` | MQTT broker address |
| `TC002_MQTT_TOPIC` | Device topic, in the form `[prefix]_[last 4 digits of MAC]/custom/claude_bot` |
| `TC002_DURATION` | Display duration (seconds), default 31536000 (one year, so it stays on permanently) |

## MQTT Payload

This app publishes a TC002 Custom App JSON payload. The image is sent through the `image` field as an inline base64 data URL, so there is no need to upload the image separately or to use an external image host.

Example structure:

```json
{
  "duration": 86400,
  "text": [],
  "image": [
    {
      "data": "data:image/gif;base64,...",
      "position": [0, 0]
    }
  ],
  "draw": []
}
```

## Testing on Real Hardware

For detailed steps see `HOME_ASSISTANT_TESTING.md`.

Quick checklist:

1. Check the TC002's current IP and MQTT configuration:

   ```bash
   curl http://<TC002_IP>/getBase
   curl http://<TC002_IP>/getMqttConfig
   ```

2. Determine the topic. For example, if the MQTT prefix is `ulanzi` and the last four digits of the device MAC are `1bf6`, the topic is:

   ```text
   ulanzi_1bf6/custom/claude_bot
   ```

3. Make sure the TC002 is currently showing the corresponding custom app, e.g. `claude_bot`.

4. Publish real data:

   ```bash
   cd apps/mqtt/claude-bot
   bash lab/publish_usage.sh
   ```

## Known Issues

- After receiving a Custom App update, the TC002 does not necessarily switch to that app automatically. When testing, it is best to switch to the target custom app manually on the device first, or publish to the name of the app currently being displayed.
- When recording the TC002 with a phone camera, the video may show flicker or horizontal dark bands rolling across the screen. This is usually caused by the LED matrix scan refresh / PWM dimming not being in sync with the camera shutter, and does not necessarily mean the image flickers to the naked eye.
- The TC002's MQTT topic rules may change with official firmware updates, so the topic has been made configurable.

## License

GPL-3.0-or-later.
