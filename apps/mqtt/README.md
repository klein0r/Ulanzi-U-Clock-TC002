# MQTT Apps

This directory collects community-contributed **MQTT apps** — mostly [Home Assistant blueprints](https://www.home-assistant.io/docs/blueprint/) that users can import into their own Home Assistant instance with one click and then use to interact with the Ulanzi TC002 pixel clock over MQTT (push notifications, display sensor data, play icon animations and so on). This directory is the detail page for the [MQTT Apps](../../README.md#4-mqtt-apps) section of the top-level [README.md](../../README.md).

> Other MQTT-based integration approaches are equally welcome, for example Node-RED flows, openHAB rules, ESPHome automations and so on.

---

## Apps Included

### [xiaohongshu-follower-counter](xiaohongshu-follower-counter/)

<img src="xiaohongshu-follower-counter/preview/demo.png" width="416" alt="Xiaohongshu follower count 52×16 preview">

> **Fully local Xiaohongshu follower count display** — Chrome uses the user's own login session to read the follower count visible on a profile page and publishes it to the TC002 through a Home Assistant webhook and the MQTT integration, without relying on any third-party scraping cloud service.

| | |
|---|---|
| **Type** | Chrome MV3 extension + Home Assistant MQTT blueprint |
| **Privacy** | Does not read cookies; does not store MQTT credentials or an HA long-lived token |
| **Documentation** | [xiaohongshu-follower-counter/docs/README.md](xiaohongshu-follower-counter/docs/README.md) |

---

### [vibe-coding-signal-light](vibe-coding-signal-light/)

<img src="vibe-coding-signal-light/preview/demo.gif" width="416" alt="vibe-coding-signal-light preview (original 52×16 pixels, scaled 8×)">

> **A TC002 desktop status traffic light** — shows the run state of AI coding assistants such as Claude Code / Codex / CI (`idle` / `working` / `attention` / `blocked` / `off`)

| | |
|---|---|
| **Type** | Home Assistant blueprint |
| **Author** | 王行知 ([@castlewong](https://github.com/castlewong)) |
| **Import** | [![Open in HA](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fraw.githubusercontent.com%2FUlanziTechnology%2FUlanzi-U-Clock-TC002%2Fmain%2Fapps%2Fmqtt%2Fvibe-coding-signal-light%2Fblueprint.yaml) |
| **Documentation** | [vibe-coding-signal-light/docs/README.md](vibe-coding-signal-light/docs/README.md) |

---

**More apps**:

| App | Description |
| --- | --- |
| [ci-status-board](ci-status-board/) | CI build status board |
| [claude-bot](claude-bot/) | Claude status and usage display |
| [fire](fire/) | Virtual fireplace |
| [git-contribution-heatmap](git-contribution-heatmap/) | Git contribution heatmap |
| [love-confession](love-confession/) | Fan light board |
| [nowplaying](nowplaying/) | Now playing (marquee) |
| [pet](pet/) | Desktop pixel pet (grey cat) |
| [vocabulary-widget](vocabulary-widget/) | Vocabulary carousel |
| [year-progress-bar](year-progress-bar/) | Year progress bar |

> 👀 Looking forward to more community work — see below for how to submit.

---

## What These Apps Can Do

A typical MQTT app consists of:

1. **A trigger** — for example a state change of some HA entity, a schedule, or a sensor crossing a threshold
2. **An MQTT action** — calling `mqtt.publish` to send the content to be shown on the TC002 (text, icons, colors, duration) to the topic the device subscribes to
3. **Optional icon resources** — 8×8 pixel PNG/GIF images, flashed to the device's `/icons/` directory in advance and referenced from the payload

Examples of common uses:

- 📨 New email / IM message alerts
- 🔋 Low battery notifications for phones / smart locks / smoke detectors
- 🌡️ Scrolling indoor and outdoor temperature, humidity and air quality values
- 🎵 Media player cover art + title changes
- 🚌 Live bus arrival countdowns
- 📈 Stock / cryptocurrency prices
- ⏰ Pomodoro timer / countdown

---

## Submitting Your App

### 1. Read the Full Guidelines

Before submitting, please read [`CONTRIBUTING.md`](../../CONTRIBUTING.md) in the project root. The key points:

- **Directory structure**: `apps/mqtt/<your-app-name>/`, containing `blueprint.yaml` + `preview/` + `docs/`, and optionally `icons/`
- **App name**: lowercase letters + digits + hyphens, for example `battery-monitor`, `weather-card`
- **License**: must be compatible with the repository's main license (GPL-3.0-or-later)
- **Metadata**: written in the SPDX comment at the top of `blueprint.yaml` and in the `blueprint:` section; no separate `manifest.json` is needed
- **Real-device verification**: the screenshots or GIFs in `preview/` must show the app running on a real TC002

### 2. Minimal Directory Skeleton

```
apps/mqtt/<your-app-name>/
├── blueprint.yaml      # The main HA blueprint file
├── preview/
│   └── demo.gif        # The app running on real hardware
└── docs/
    └── README.md       # Includes the "Open in HA" one-click import button + parameter descriptions + the list of MQTT topics
```

### 3. blueprint.yaml Starting Point

```yaml
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Your Name <your@email.com>

blueprint:
  name: A one-line title
  description: |
    A detailed description of what the app does.
    Author: Your Name
    License: GPL-3.0-or-later
  domain: automation
  source_url: https://github.com/UlanziTechnology/Ulanzi-U-Clock-TC002/blob/main/apps/mqtt/<your-app-name>/blueprint.yaml
  input:
    # The parameters exposed to the user (broker, topic, thresholds, display text, etc.)

trigger:
  # When it fires

action:
  - service: mqtt.publish
    data:
      topic: tc002/notify
      payload: '{"text":"Hello","icon":1,"duration":5}'
```

### 4. Required Elements of docs/README.md

- An **"Open in HA" one-click import button** embedded at the top (clicking it takes an HA user straight to the import dialog in their own instance)
- At least one GIF / screenshot (it may reference `../preview/`)
- The **list of all MQTT topics** the blueprint publishes to / subscribes to, plus **example payloads**
- A description of the configuration parameters (matching `blueprint.input`)
- Troubleshooting / known issues

Markdown template for the "Open in HA" button:

```markdown
[![Open in HA](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FUlanziTechnology%2FUlanzi-U-Clock-TC002%2Fblob%2Fmain%2Fapps%2Fmqtt%2F<your-app-name>%2Fblueprint.yaml)
```

Replace every `/` in the URL with `%2F`, and replace `<your-app-name>` with your directory name.

### 5. Open a Pull Request

```bash
git checkout -b mqtt/<your-app-name>
# ... finish development ...
git push origin mqtt/<your-app-name>
```

Then open a PR on GitHub from your branch to `UlanziTechnology/Ulanzi-U-Clock-TC002:main`, and tick the PR checklist at the end of [`CONTRIBUTING.md`](../../CONTRIBUTING.md) in the description.

---

## MQTT Communication Conventions for the TC002

The Custom App MQTT conventions of the official TC002 firmware are already defined (see the [MQTT Apps section of the top-level README.md](../../README.md#4-mqtt-apps)):

- **Topic format**: `[PREFIX]/custom/[APP_NAME]`, where `[PREFIX]` is the MQTT prefix + the last four digits of the device MAC (by default something like `ulanzi_1bf6`; the device's MQTT configuration is authoritative), and `[APP_NAME]` is the name of the custom app on the TC002
- **Payload**: UTF-8 JSON, supporting fields such as `text` (text), `image` (inline base64 PNG/GIF), `draw` (vector drawing) and `duration` (display seconds); for the full structure and examples see the main README

When submitting an app you must still follow these rules:

- The broker address / username / password must all be parameterized through `blueprint.input` — **do not hard-code them**
- List all topics your blueprint publishes to / subscribes to, with example payloads, in `docs/README.md`
- Always encode payloads as UTF-8 JSON

---

## Feedback and Discussion

- **Submitting an app**: see "Submitting Your App" above
- **Reporting problems / making suggestions**: [GitHub Issues](https://github.com/UlanziTechnology/Ulanzi-U-Clock-TC002/issues)
- **Full contribution guidelines**: [`../../CONTRIBUTING.md`](../../CONTRIBUTING.md)
- **Official website**: <https://www.ulanzi.com>
