# TC002 Vocabulary Carousel — Quick Start

Get the TC002 showing English words in 5 minutes.

## Prerequisites

- [x] Python 3 + Pillow installed
- [x] mosquitto installed
- [x] TC002 device powered on and connected to Wi-Fi

## Step 1: Get the Device Information

```bash
curl http://<device IP>/getBase
curl http://<device IP>/getMqttConfig
```

## Step 2: Work Out the MQTT Topic

```
[mqtt_prefix]_[last 4 digits of MAC]/custom/vocabulary
```

## Step 3: Test the MQTT Connection

```bash
mosquitto_pub -h <broker address> -t <your topic> -m '{"duration":31536000,"text":[],"image":[],"draw":[{"df":[0,0,52,16,"#00FF00"]}]}'
```

## Step 4: Send a Word

```bash
cd apps/mqtt/vocabulary-widget

# A specific word:
B64=$(python3 lab/render_vocabulary.py --word hello) && mosquitto_pub -h <broker> -t <topic> -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"
```

## Step 5: Use Your Own Word List

```bash
# Prepare a CSV file, then:
B64=$(python3 lab/render_vocabulary.py --source /path/to/words.csv --count 3) && mosquitto_pub -h <broker> -t <topic> -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"
```

## FAQ

### Q1: Nothing shows on the screen

- Check that the device IP and the MQTT broker are correct
- Manually switch to the `vocabulary` app on the device

### Q2: The Chinese text is cut off

- Chinese definitions are automatically truncated to the first two lines
- Keep the definitions short
