# TC002 Claude Bot — Real-Device Testing Guide

This document explains how to test `claude-bot` on real TC002 (U-Clock) hardware and verify the display of real Claude Code rate limits.

## 1. Basic Concept

```text
Claude Code → statusLine hook → MQTT broker → TC002
```

## 2. MQTT Broker

You need an MQTT broker that both the TC002 and your computer can reach.

Local testing on a Mac:

```bash
brew install mosquitto
brew services start mosquitto
```

## 3. Configure MQTT on the TC002

In the TC002's MQTT settings, enter:

```text
Broker host: <broker IP>
Broker port: 1883
MQTT prefix: ulanzi
```

To view the TC002's current configuration:

```bash
curl http://<TC002_IP>/getMqttConfig
```

## 4. Custom App Topic

Format:

```text
[PREFIX]/custom/[APP_NAME]
```

Example:

```text
ulanzi_1bf6/custom/claude_bot
```

## 5. Test the MQTT Connection

```bash
# Turn the screen green (to verify the connection)
mosquitto_pub -h <BROKER_IP> -t ulanzi_1bf6/custom/claude_bot \
  -m '{"duration":31536000,"text":[],"image":[],"draw":[{"df":[0,0,52,16,"#00FF00"]}]}'
```

## 6. Configure the statusLine Hook

1. Add the following to `~/.claude/settings.json`:

   ```json
   {
     "statusLine": {
       "type": "command",
       "command": "node /path/to/Ulanzi-U-Clock-TC002/apps/mqtt/claude-bot/lab/claude_statusline_bridge.js"
     }
   }
   ```

2. Restart Claude Code; the hook is triggered automatically after you send any message.

3. Check the state file to verify:

   ```bash
   cat /tmp/claude-statusline-state.json
   ```

## 7. Test the Real Usage Display

```bash
cd apps/mqtt/claude-bot

# Read from the state file and publish:
TC002_MQTT_HOST=<broker IP> bash lab/publish_usage.sh
```

The TC002 should show Claude Bot + 5H:XX% / 7d:XX% (real rate limit data).

## 8. Step-by-Step Debugging

```bash
# 1. Render manually (without publishing)
python3 lab/render_usage.py 50 30 --file /tmp/claude_bot_usage.gif

# 2. Publish the rendered base64 manually
python3 lab/render_usage.py 50 30 | xargs -I{} mosquitto_pub \
  -h <BROKER_IP> -t ulanzi_1bf6/custom/claude_bot \
  -m '{"duration":31536000,"text":[],"image":[{"data":"data:image/gif;base64,{}","position":[0,0]}],"draw":[]}'
```

## 9. Polling Mode

```bash
TC002_MQTT_HOST=<broker IP> bash lab/publish_usage.sh --loop 300
```

## 10. Troubleshooting

### MQTT publishes successfully but nothing changes on the TC002

- Check that the TC002 and the broker are on the same local network
- Check that the topic prefix matches the last four digits of the device's MAC address
- Manually switch to the target custom app on the TC002

### The broker works but the TC002 receives nothing

- Check the macOS firewall
- Check for AP isolation on your router
