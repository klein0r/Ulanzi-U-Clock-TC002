# TC002 CI Status Board — Quick Start

Get the TC002 showing your GitHub Actions CI status in 5 minutes.

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
[mqtt_prefix]_[last 4 digits of MAC]/custom/ci_status
```

## Step 3: Test the MQTT Connection

```bash
mosquitto_pub -h <broker address> -t <your topic> -m '{"duration":31536000,"text":[],"image":[],"draw":[{"df":[0,0,52,16,"#00FF00"]}]}'
```

## Step 4: Send the CI Status

```bash
cd apps/mqtt/ci-status-board

# Set the GitHub repository to monitor
export GITHUB_REPO=owner/repo

# Fetch the real status:
B64=$(python3 lab/render_ci_status.py) && mosquitto_pub -h <broker> -t <topic> -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"
```

## Step 5: Use a Home Assistant Automation (Recommended)

Import `blueprint.yaml` and fill in the configuration:
- **GitHub repository**: `owner/repo`
- **GitHub token** (optional): required for private repositories

The blueprint automatically checks and updates the CI status every 5 minutes.

## FAQ

### Q1: Nothing shows on the screen

- Check that the device IP and the MQTT broker are correct
- Manually switch to the `ci_status` app on the device

### Q2: No data is fetched

- Check that `GITHUB_REPO` has the right format (owner/repo)
- Private repositories require `GITHUB_TOKEN` to be set
