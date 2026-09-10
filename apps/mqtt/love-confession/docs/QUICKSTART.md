# TC002 Fan Light Board — Quick Start

Get the TC002 showing heartbeat/firework/shooting-star animations in 5 minutes.

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
[mqtt_prefix]_[last 4 digits of MAC]/custom/love_confession
```

## Step 3: Test the MQTT Connection

```bash
mosquitto_pub -h <broker address> -t <your topic> -m '{"duration":31536000,"text":[],"image":[],"draw":[{"df":[0,0,52,16,"#FF306E"]}]}'
```

Screen turns pink = connection successful.

## Step 4: Send an Animation

```bash
cd apps/mqtt/love-confession

# Heartbeat:
B64=$(python3 lab/render_confession.py heart) && mosquitto_pub -h <broker> -t <topic> -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"

# Firework:
B64=$(python3 lab/render_confession.py firework) && mosquitto_pub -h <broker> -t <topic> -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"

# Shooting star:
B64=$(python3 lab/render_confession.py star) && mosquitto_pub -h <broker> -t <topic> -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"

# Scrolling text:
B64=$(python3 lab/render_confession.py text "I LOVE YOU") && mosquitto_pub -h <broker> -t <topic> -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"
```

## FAQ

### Q1: Nothing shows on the screen

- Check that the device IP and the MQTT broker are correct
- Manually switch to the `love_confession` app on the device
