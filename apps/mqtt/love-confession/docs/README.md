# TC002 Fan Light Board

> **New here?** See [QUICKSTART.md](QUICKSTART.md)

## Introduction

Turns the TC002 (U-Clock) into a confession gadget / concert fan light board.

- Heartbeat animation
- Bursting fireworks
- Shooting stars
- Custom scrolling text

**Author**: 王行知

## Preview

See the GIFs in the `preview/` directory

## Dependencies

- Python 3 + Pillow (`pip install pillow`)
- mosquitto_pub (`brew install mosquitto`)
- An MQTT broker (reachable from both the TC002 and your computer)

## Installation

1. Add the environment variables to `~/.zshrc`:

   ```bash
   export TC002_MQTT_HOST=<your MQTT broker address>
   export TC002_MQTT_TOPIC=ulanzi_1bf6/custom/love_confession
   ```

## Manual Testing

```bash
cd apps/mqtt/love-confession

# Heartbeat animation:
B64=$(python3 lab/render_confession.py heart) && mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"

# Firework animation:
B64=$(python3 lab/render_confession.py firework) && mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"

# Shooting star animation:
B64=$(python3 lab/render_confession.py star) && mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"

# Scrolling text:
B64=$(python3 lab/render_confession.py text "I LOVE YOU") && mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"
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

## License

GPL-3.0-or-later.
