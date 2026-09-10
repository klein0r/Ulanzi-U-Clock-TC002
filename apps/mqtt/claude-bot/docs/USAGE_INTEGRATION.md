# Claude Code Usage Integration

This document explains how `claude-bot` hooks into real Claude Code rate limit data.

> **Full installation guide**: see [QUICKSTART.md](QUICKSTART.md)

## Data Flow

```text
Claude Code statusLine hook
  → claude_statusline_bridge.js (reads the JSON from stdin, extracts the quota percentages)
  → render_usage.py (renders the 52x16 GIF)
  → mosquitto_pub (publishes to the TC002 over MQTT)
```

## Installation Steps

### Prerequisites

- [x] Claude Code installed
- [x] Python 3 + Pillow installed
- [x] mosquitto installed (the MQTT client)
- [x] TC002 device powered on and connected to Wi-Fi
- [x] You know the device IP and the MQTT broker address

### 1. Get the Device Information

```bash
# Get the basic device information (MAC address)
curl http://<device IP>/getBase

# Get the MQTT configuration (broker address, prefix)
curl http://<device IP>/getMqttConfig
```

### 2. Work Out the MQTT Topic

```
[mqtt_prefix]_[last 4 digits of MAC]/custom/claude_bot
```

Example: `ulanzi_1bf6/custom/claude_bot`

### 3. Clone the Repository

```bash
git clone https://github.com/UlanziTechnology/Ulanzi-U-Clock-TC002.git
cd Ulanzi-U-Clock-TC002
```

### 4. Configure the Environment Variables

Add the following at the end of `~/.zshrc` (macOS) or `~/.bashrc` (Linux):

```bash
# TC002 Claude Bot — MQTT configuration
export TC002_MQTT_HOST=<your MQTT broker address>
export TC002_MQTT_PORT=1883
export TC002_MQTT_TOPIC=<your device topic>
export TC002_DURATION=31536000
```

Reload it:
```bash
source ~/.zshrc  # or source ~/.bashrc
```

### 5. Configure the Claude Code statusLine Hook

Edit `~/.claude/settings.json` and add:

```json
{
  "statusLine": {
    "type": "command",
    "command": "node /path/to/your/repo/apps/mqtt/claude-bot/lab/claude_statusline_bridge.js"
  }
}
```

### 6. Test the MQTT Connection

```bash
mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m '{"duration":31536000,"text":[],"image":[],"draw":[{"df":[0,0,52,16,"#00FF00"]}]}'
```

Screen turns green = connection successful.

### 7. Restart Claude Code and Verify

```bash
# Restart Claude Code, send any message, then check:
cat /tmp/claude-statusline-state.json
```

### 8. Trigger a Publish Manually

```bash
cd apps/mqtt/claude-bot
bash lab/publish_usage.sh
```

The TC002 should show Claude Bot + 5H:0% / 7d:0%.

## What Is Displayed

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

## Division of Labor Between the Scripts

| Script | Responsibility | Usable standalone |
|---|---|---|
| `claude_statusline_bridge.js` | Receives the statusLine JSON, extracts the quotas, renders the GIF, publishes over MQTT | ✅ |
| `render_usage.py` | Takes the percentages, renders a 52×16 animated GIF, outputs base64 | ✅ |
| `publish_usage.sh` | Reads the percentages from the state file or manual input and publishes over MQTT | ✅ |

## Manual Testing

```bash
cd apps/mqtt/claude-bot

# Read from the state file and publish (recommended):
bash lab/publish_usage.sh

# Polling mode (every 300 seconds):
bash lab/publish_usage.sh --loop 300
```

## MQTT Payload

The bridge publishes a TC002 Custom App JSON payload. The image is sent through the `image` field as an inline base64 data URL.

Example structure:

```json
{
  "duration": 31536000,
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

`duration` defaults to 31536000 seconds (one year), so the image stays on screen instead of the display turning off.

## FAQ

### Q1: The state file shows 0%/0%

**Possible causes**:
- The Claude subscription has just reset
- Claude Code is not reporting the quotas correctly (depends on the subscription type)

**How to verify**:
```bash
echo '{"rate_limits":{"five_hour":{"used_percentage":50},"seven_day":{"used_percentage":30}},"model":{"display_name":"test"}}' | node apps/mqtt/claude-bot/lab/claude_statusline_bridge.js
```

### Q2: Nothing shows on the screen

**Check**:
1. Whether the device IP is correct
2. Whether the MQTT broker address is correct
3. Whether the topic format is correct
4. Whether the device has switched to the `claude_bot` custom app

## License

GPL-3.0-or-later.
