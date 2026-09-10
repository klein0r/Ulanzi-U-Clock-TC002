# TC002 Vocabulary Carousel

> **New here?** See [QUICKSTART.md](QUICKSTART.md)

## Introduction

Turns the TC002 (U-Clock) into a small desktop screen for learning vocabulary.

- Reads words from a CSV word list
- Picks words at random or shows a specific one
- English is rendered with a pixel font, Chinese is pre-rendered with a system font

**Author**: 王行知

## Preview

See `preview/demo.gif`

## Dependencies

- Python 3 + Pillow (`pip install pillow`)
- mosquitto_pub (`brew install mosquitto`)
- An MQTT broker (reachable from both the TC002 and your computer)

## Word List Format

`.csv` files are supported; the recommended column names are:

```text
单词,音标,解释,笔记
```

The following is also supported:

```text
word,phonetic,definition
```

## Installation

1. Add the environment variables to `~/.zshrc`:

   ```bash
   export TC002_MQTT_HOST=<your MQTT broker address>
   export TC002_MQTT_TOPIC=ulanzi_1bf6/custom/vocabulary
   ```

2. Prepare your CSV word list file.

## Manual Testing

```bash
cd apps/mqtt/vocabulary-widget

# A specific word:
B64=$(python3 lab/render_vocabulary.py --word hello) && mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"

# 3 random words:
B64=$(python3 lab/render_vocabulary.py --count 3) && mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"

# Show all of them:
B64=$(python3 lab/render_vocabulary.py --all) && mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"
```

## Parameters

| Parameter | Description |
|------|------|
| `--word <word>` | Show one specific word |
| `--count <number>` | Pick N words at random |
| `--all` | Show all words |
| `--seed <number>` | Fix the random seed |
| `--source <path>` | Specify the CSV word list |

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

- Chinese definitions must not be too long; the script takes the first two lines and truncates them.
- A GIF containing the full word list is quite large and the device may not display it correctly.
- After receiving a Custom App update, the TC002 does not necessarily switch to that app automatically.

## License

GPL-3.0-or-later.
