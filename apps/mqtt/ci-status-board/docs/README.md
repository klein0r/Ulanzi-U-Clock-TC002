# TC002 CI Status Board

> **New here?** See [QUICKSTART.md](QUICKSTART.md)

## Introduction

Shows the GitHub Actions CI status on the TC002 (U-Clock).

- Green: passing
- Yellow: running
- Red: failing

**Author**: 王行知

## Preview

See `preview/demo.gif`

## Dependencies

- Python 3 + Pillow (`pip install pillow`)
- mosquitto_pub (`brew install mosquitto`)
- An MQTT broker (reachable from both the TC002 and your computer)

## Installation

### Option 1: Home Assistant Blueprint (Recommended)

1. Import `blueprint.yaml` into Home Assistant
2. Fill in the blueprint configuration:
   - **GitHub repository**: the repository to monitor, in the form `owner/repo`
   - **GitHub token** (optional): required for private repositories
   - **TC002 MQTT topic**: your device's topic
3. The blueprint automatically checks and updates the CI status every 5 minutes

### Option 2: Manual Script

1. Add the environment variables to `~/.zshrc`:

   ```bash
   export TC002_MQTT_HOST=<your MQTT broker address>
   export TC002_MQTT_TOPIC=ulanzi_1bf6/custom/ci_status
   export GITHUB_REPO=<owner/repo>
   ```

2. Optional: set a GitHub token to get the status of private repositories:

   ```bash
   export GITHUB_TOKEN=ghp_xxx
   ```

## Manual Testing

```bash
cd apps/mqtt/ci-status-board

# Fetch the real status from GitHub:
B64=$(python3 lab/render_ci_status.py) && mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"

# Demo mode:
B64=$(python3 lab/render_ci_status.py --status success --message "Fix bug") && mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"
```

## Parameters

| Parameter | Description |
|------|------|
| `--repo <owner/repo>` | Specify the GitHub repository |
| `--status <success/failure/running>` | Set the status manually (demo) |
| `--message <text>` | Set the commit message manually (demo) |

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

- The repository to monitor must be configured manually (`GITHUB_REPO`).
- After receiving a Custom App update, the TC002 does not necessarily switch to that app automatically.

## License

GPL-3.0-or-later.
