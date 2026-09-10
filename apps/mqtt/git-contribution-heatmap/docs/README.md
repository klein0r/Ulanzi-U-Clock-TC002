# TC002 Git Contribution Heatmap

> **New here?** See [QUICKSTART.md](QUICKSTART.md)

## Introduction

Shows your GitHub contribution heatmap on the TC002 (U-Clock) as a 52-column × 7-row calendar dot matrix.

- Days with commits: green at different brightness levels depending on the number of commits
- Days without commits: pure black
- Top right corner: GitHub mark + the word GITHUB

**Author**: 王行知

## Preview

See `preview/demo.gif`

## Dependencies

- Python 3 + Pillow (`pip install pillow`)
- mosquitto_pub (`brew install mosquitto`)
- An MQTT broker (reachable from both the TC002 and your computer)

## Installation

1. Add the environment variables to `~/.zshrc`:

   ```bash
   export TC002_MQTT_HOST=<your MQTT broker address>
   export TC002_MQTT_TOPIC=ulanzi_1bf6/custom/git_heatmap
   export GITHUB_USER=<your GitHub username>
   ```

2. Optional: set a GitHub token to get more complete contribution data (including private repositories):

   ```bash
   export GITHUB_TOKEN=ghp_xxx
   ```

### How to Get a GitHub Token

1. Open https://github.com/settings/tokens
2. Click **"Generate new token (classic)"**
3. Select the scopes:
   - `read:user` — read user information
   - `repo:status` — read repository status (optional, for private repository contributions)
4. Click **"Generate token"**
5. Copy the token and set it as an environment variable

> **Note**: Do not put the token into Git or commit it to a code repository. Keep it only in the current shell's environment variables.

## Manual Testing

```bash
cd apps/mqtt/git-contribution-heatmap

# Generate and publish:
B64=$(python3 lab/render_contribution_heatmap.py --user castlewong) && mosquitto_pub -h $TC002_MQTT_HOST -t $TC002_MQTT_TOPIC -m "{\"duration\":31536000,\"text\":[],\"image\":[{\"data\":\"data:image/gif;base64,$B64\",\"position\":[0,0]}],\"draw\":[]}"

# Demo mode (does not contact GitHub):
python3 lab/render_contribution_heatmap.py --demo --output preview/demo.gif
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

- Without a GitHub token, only recent public PushEvents can be retrieved.
- After receiving a Custom App update, the TC002 does not necessarily switch to that app automatically.

## License

GPL-3.0-or-later.
