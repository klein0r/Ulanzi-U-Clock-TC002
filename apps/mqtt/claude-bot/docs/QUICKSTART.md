# Claude Bot Quick Start

This document helps you get the TC002 (U-Clock) showing your real Claude Code quota usage in 5 minutes.

## Prerequisites

- [x] Claude Code installed
- [x] Python 3 installed (`python3 --version`)
- [x] pip installed (`pip3 --version`)
- [x] TC002 device powered on and connected to Wi-Fi

## Step 1: Install the Dependencies

```bash
# Install the Python Pillow library (used to render the GIF)
pip3 install pillow

# Install mosquitto (the MQTT client)
# macOS:
brew install mosquitto

# Ubuntu/Debian:
sudo apt install mosquitto-clients
```

## Step 2: Get the Device Information

```bash
# Find your TC002's IP address
# Option 1: look it up in the TC002 menu
# Option 2: check your router's admin page

# Assuming the device IP is 10.19.1.128, get the device information:
curl http://10.19.1.128/getBase
```

Example output:
```json
{
  "devSn": "B0D191008U3670007",
  "ssid": "your Wi-Fi",
  "ip": "10.19.1.128",
  "mac": "ccc4b2441bf6",  ← remember this MAC address
  "mcuVer": "T1.0.13",
  "appVer": "0.2.9"
}
```

```bash
# Get the MQTT configuration:
curl http://10.19.1.128/getMqttConfig
```

Example output:
```json
{
  "isMqtt": true,
  "ip": "10.19.1.58",      ← this is your MQTT broker address
  "port": "1883",
  "mqtt_prefix": "ulanzi",  ← remember this prefix
  "mqtt_name": "",
  "mqtt_pwd": ""
}
```

## Step 3: Work Out Your MQTT Topic

Using the information collected above, work out the topic:

```
[mqtt_prefix]_[last 4 digits of MAC]/custom/claude_bot
```

Example:
- MQTT prefix: `ulanzi`
- Last 4 digits of the MAC: `1bf6`
- Topic: `ulanzi_1bf6/custom/claude_bot`

## Step 4: Clone the Repository

```bash
git clone -b docs/translate-to-english https://github.com/klein0r/Ulanzi-U-Clock-TC002.git
cd Ulanzi-U-Clock-TC002
```

## Step 5: Configure the Environment Variables

Add the following at the end of `~/.zshrc` (macOS) or `~/.bashrc` (Linux):

```bash
# TC002 Claude Bot — MQTT configuration
export TC002_MQTT_HOST=10.19.1.58        # your MQTT broker address (from getMqttConfig)
export TC002_MQTT_PORT=1883              # MQTT port (1883 by default)
export TC002_MQTT_TOPIC=ulanzi_1bf6/custom/claude_bot  # your device topic (worked out in step 3)
export TC002_DURATION=31536000           # display duration (seconds), one year by default, so it stays on permanently
```

Then reload the configuration:

```bash
source ~/.zshrc  # or source ~/.bashrc
```

## Step 6: Configure the Claude Code statusLine Hook

Edit `~/.claude/settings.json` and add a `statusLine` field:

```json
{
  "statusLine": {
    "type": "command",
    "command": "node /path/to/your/repo/apps/mqtt/claude-bot/lab/claude_statusline_bridge.js"
  }
}
```

**Note**: replace `/path/to/your/repo/` with the actual path, for example:

```json
{
  "statusLine": {
    "type": "command",
    "command": "node /Users/yourname/Ulanzi-U-Clock-TC002/apps/mqtt/claude-bot/lab/claude_statusline_bridge.js"
  }
}
```

## Step 7: Test the MQTT Connection

```bash
# Test whether you can connect to the MQTT broker
mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m '{"duration":31536000,"text":[],"image":[],"draw":[{"df":[0,0,52,16,"#00FF00"]}]}'
```

If the TC002's screen turns green, the MQTT connection works.

## Step 8: Restart Claude Code and Verify

```bash
# Restart Claude Code
# Then send any message

# Check whether the state file has been created:
cat /tmp/claude-statusline-state.json
```

You should see something like:
```json
{
  "timestamp": "2026-06-25T...",
  "rate_limits": {
    "five_hour_pct": 0,
    "seven_day_pct": 0
  }
}
```

## Step 9: Trigger a Publish Manually

```bash
cd apps/mqtt/claude-bot
bash lab/publish_usage.sh
```

The TC002 should show the Claude Bot mascot + 5H:0% / 7d:0%.

## FAQ

### Q1: Nothing shows on the TC002 screen

**Check**:
1. That the device IP is correct: `curl http://<device IP>/getBase`
2. That the MQTT broker address is correct: `curl http://<device IP>/getMqttConfig`
3. That the topic format is correct: `[prefix]_[last 4 digits of MAC]/custom/claude_bot`
4. Manually switch to the `claude_bot` custom app on the device

### Q2: The state file shows 0%/0%

**Possible causes**:
- The Claude subscription has just reset
- Claude Code is not reporting the quotas correctly (depends on the subscription type)

**How to verify**: test the chain with simulated data:
```bash
echo '{"rate_limits":{"five_hour":{"used_percentage":50},"seven_day":{"used_percentage":30}},"model":{"display_name":"test"}}' | node apps/mqtt/claude-bot/lab/claude_statusline_bridge.js
```

### Q3: MQTT publishing fails

**Check**:
1. Whether the MQTT broker is running: `mosquitto_pub -h $TC002_MQTT_HOST -t test -m "hello"`
2. Whether the device is on the same local network
3. Whether a firewall is blocking the connection

### Q4: The screen turns off

**Solution**: make sure `TC002_DURATION` is set to a large enough value (31536000 seconds = one year by default).

## Done

From now on, every time you use Claude Code the TC002 shows your 5-hour/7-day quota usage in real time.

- Green (< 70%): normal
- Yellow (70-90%): caution
- Red (> 90%): danger
