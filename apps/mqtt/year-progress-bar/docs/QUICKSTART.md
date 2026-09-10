# TC002 Year Progress Bar — Quick Start

Get the TC002 showing this year's progress in 5 minutes.

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
[mqtt_prefix]_[last 4 digits of MAC]/custom/year_progress
```

## Step 3: Test the MQTT Connection

```bash
mosquitto_pub -h <broker address> -t <your topic> -m '{"duration":31536000,"text":[],"image":[],"draw":[{"df":[0,0,52,16,"#00FF00"]}]}'
```

Screen turns green = connection successful.

## Step 4: Publish the Year Progress Bar

```bash
cd apps/mqtt/year-progress-bar

# Generate and publish:
B64=$(python3 lab/render_year_progress.py) && mosquitto_pub -h <broker address> -t <your topic> -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"
```

The TC002 should now show the year progress bar (48%).

## Step 5: Scheduled Updates (Optional)

A Home Assistant blueprint can update the progress bar automatically every day. Just import `blueprint.yaml`.

## FAQ

### Q1: Nothing shows on the screen

- Check that the device IP is correct
- Check that the MQTT broker address is correct
- Manually switch to the `year_progress` custom app on the device

### Q2: The progress does not update

- By default the blueprint updates at 8 a.m. every day
- You can run the script manually to refresh

## Done

The TC002 now shows this year's progress bar, updated automatically every day.
