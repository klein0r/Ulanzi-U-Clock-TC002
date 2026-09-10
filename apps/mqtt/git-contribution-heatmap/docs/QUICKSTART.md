# TC002 Git Contribution Heatmap — Quick Start

Get the TC002 showing your GitHub contribution heatmap in 5 minutes.

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
[mqtt_prefix]_[last 4 digits of MAC]/custom/git_heatmap
```

## Step 3: Test the MQTT Connection

```bash
mosquitto_pub -h <broker address> -t <your topic> -m '{"duration":31536000,"text":[],"image":[],"draw":[{"df":[0,0,52,16,"#00FF00"]}]}'
```

## Step 4: Publish the Heatmap

```bash
cd apps/mqtt/git-contribution-heatmap

# Set your GitHub username
export GITHUB_USER=your-username

# Generate and publish:
B64=$(python3 lab/render_contribution_heatmap.py) && mosquitto_pub -h <broker address> -t <your topic> -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"
```

## Step 5: Scheduled Updates (Optional)

A Home Assistant blueprint can update it automatically every day. Just import `blueprint.yaml`.

Or use cron:
```bash
# Update at 9 a.m. every day
0 9 * * * cd /path/to/apps/mqtt/git-contribution-heatmap && B64=$(python3 lab/render_contribution_heatmap.py) && mosquitto_pub -h <broker> -t <topic> -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"
```

## FAQ

### Q1: No data is fetched

- Check that the GitHub username is correct
- Try `--demo` mode first to test
- Set `GITHUB_TOKEN` to get more complete data

### Q2: Nothing shows on the screen

- Check that the device IP and the MQTT broker are correct
- Manually switch to the `git_heatmap` app on the device
