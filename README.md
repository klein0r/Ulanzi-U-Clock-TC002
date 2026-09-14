# Ulanzi U-Clock TC002 · Pixel Clock Open Source Resources

> [!NOTE]
> **This is an AI-translated fork.** The documentation, code comments and UI strings in
> this repository were machine-translated from Chinese into English. The translation may
> lag behind the original, and it may contain mistakes or subtly inaccurate wording —
> please do not treat it as authoritative.
>
> For the original, canonical repository see
> [UlanziTechnology/Ulanzi-U-Clock-TC002](https://github.com/UlanziTechnology/Ulanzi-U-Clock-TC002).
> If anything here is unclear or looks wrong, check the original source first.

The Ulanzi TC002 (Pixbar 2nd generation) is a **52×16 full-color RGB pixel desktop clock**: out of the box it can show weather, a pomodoro timer, social media follower counts and more through the official app, while also exposing the **MQTT / HTTP protocols**, so it can be connected to Home Assistant or Node-RED, or even turn the run state of Claude Code / Codex into a desktop traffic light. This repository holds the official open source resources: the sample project, MQTT apps, protocol documentation and development guides.

> **Pick an entry point based on your goal**
> - No coding, just want to play with it → [Quick Start](#2-quick-start)
> - Integrating with Home Assistant / smart home → [MQTT Apps](#4-mqtt-apps)
> - A status traffic light for Claude Code / Codex → [Agent Setup](#6-agent-setup)
> - Developing your own firmware app → [Custom Development](#7-custom-development)

## 1. Product Overview

| Item       | Specification                                     |
| ---------- | ---------------------------------------- |
| Display       | 52×16 full-color RGB LED matrix (832 LEDs), SPI driven |
| Input       | 1 knob (clockwise / counter-clockwise / press) + 3 buttons            |
| Audio / MIC | Built-in speaker (MP3, volume levels 0~6); the microphone level is reported by the MCU |
| Wireless       | Wi-Fi + BLE (BLE requires Wi-Fi to be enabled)        |
| Expansion       | GPIO_06 / GPIO_85 reserved; USB-C (mass storage mode) + reset button |

The official app (Ulanzi Studio V3.1.0+) works out of the box: time / weather / world clock, pomodoro timer / scoreboard / stopwatch / BUSY, social media follower counts, calendar, DIY pixel art. For instructions see the [official Ulanzi Studio guide](https://docs.ulanzistudio.com/tc002/en/software/ulanzi-studio.html) and the [TC002 FAQ](https://docs.ulanzistudio.com/tc002/en/faq/).

## 2. Quick Start

Prerequisite: the TC002 has been connected to Wi-Fi and is on the same local network as your computer. For network setup problems, see the [Web Setup Guide](https://docs.ulanzistudio.com/tc002/en/software/web-setup.html).

**Route A · Try it in the browser (5 minutes, no firmware flashing)** — use [PixDeck](https://github.com/cailurus/PixDeck) to push market data / weather / games / pixel art to the clock from your browser:

```
git clone https://github.com/cailurus/PixDeck.git && cd PixDeck
python3 pixbar_panel.py        # Windows: python pixbar_panel.py
# Open http://127.0.0.1:8000 in your browser and enter the clock's IP under the gear icon in the top right
```

**Route B · MQTT + Home Assistant** — configure MQTT in Ulanzi Studio (computer IP:1883) → add the MQTT integration in HA pointing at the same broker → import a community blueprint or publish payloads directly. See [MQTT Apps](#4-mqtt-apps).

**Route C · Agent status light** — the TC002 lights up while Claude Code / Codex runs (yellow = running, red = blocked, green = finished). See [Agent Setup](#6-agent-setup).

**Route D · Custom development** — import `Z21_TC002_Demo/` into the FlyThings IDE, build and run. See [Custom Development](#7-custom-development).

## 3. What Is Open Source Here

```
├── ide-guide/       FlyThings IDE development documentation
├── Z21_TC002_Demo/  The official sample project (covering LED/buttons/audio/MIC/Wi-Fi/BLE/GPIO), import into the IDE and build directly
├── apps/
│   ├── flythings/   Device-side C++ apps (pixel-pet-display and others)
│   └── mqtt/        Community MQTT apps (11 Home Assistant blueprints and more)
├── CONTRIBUTING.md  Contribution guidelines and PR process
├── LICENSE          GPL-3.0-or-later
└── THIRD_PARTY_NOTICES.md  Third-party component notices
```

> `apps/flythings/` already contains the first community app, [pixel-pet-display](apps/flythings/pixel-pet-display/) (a pixel pet display window). More C++ apps that run on the device are very welcome — see [Community and Contributing](#9-community-and-contributing).

## 4. MQTT Apps

The official TC002 firmware has MQTT built in, so text / icons / drawing commands can be pushed to the clock through messages.

**Topic**: `[PREFIX]/custom/[APP_NAME]` (the prefix defaults to `ulanzi_<last 4 digits of MAC>`, e.g. `ulanzi_1bf6` — the device's MQTT configuration is authoritative; `APP_NAME` is the name of the custom app on the clock)

**Payload (UTF-8 JSON)**:

```json
{
  "duration": 3600,
  "text": [
    {
      "content": "Hello World",
      "fontHeight": 10,
      "x": 0,
      "y": 0,
      "color": "#FFFFFF",
      "align": "left",
      "valign": "top",
      "rect": [0, 0, 52, 16],
      "charSpacing": 1
    }
  ],
  "image": [
    {
      "data": "data:image/png;base64,...",
      "position": [0, 0]
    }
  ],
  "draw": []
}
```

* `text` is the text and its layout; `image` is an inline base64 PNG/GIF (no image host required); `draw` is vector drawing (`{"df":[0,0,52,16,"#000000"]}` fills a rectangle, `{"dfc":[26,8,5,"#FFCB52"]}` fills a circle); `duration` is the number of seconds to display.

**Quick check** (with any MQTT client):

```
mosquitto_pub -h <BROKER_HOST> -t ulanzi_1bf6/custom/vibe_signal \
 -m '{"duration":3600,"text":[{"content":"Hello","fontHeight":10,"x":0,"y":0,"color":"#FFFFFF"}],"image":[],"draw":[]}'
```

**Community apps** (for the full list and how to import them, see [apps/mqtt/README.md](apps/mqtt/README.md)):

| App | Purpose |
| --- | --- |
| [vibe-coding-signal-light](apps/mqtt/vibe-coding-signal-light/) | A status traffic light for AI coding assistants ([one-click import](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fraw.githubusercontent.com%2Fklein0r%2FUlanzi-U-Clock-TC002%2Fdocs%2Ftranslate-to-english%2Fapps%2Fmqtt%2Fvibe-coding-signal-light%2Fblueprint.yaml)) |
| [claude-bot](apps/mqtt/claude-bot/) | Claude status and usage display |
| [pet](apps/mqtt/pet/) | Desktop pixel pet (grey cat) |
| [fire](apps/mqtt/fire/) | Virtual fireplace |
| [ci-status-board](apps/mqtt/ci-status-board/) | CI build status board |
| [git-contribution-heatmap](apps/mqtt/git-contribution-heatmap/) | Git contribution heatmap |
| [year-progress-bar](apps/mqtt/year-progress-bar/) | Year progress bar |
| [nowplaying](apps/mqtt/nowplaying/) | Media now playing (marquee) |
| [vocabulary-widget](apps/mqtt/vocabulary-widget/) | Vocabulary carousel |
| [love-confession](apps/mqtt/love-confession/) | Fan light board |
| [xiaohongshu-follower-counter](apps/mqtt/xiaohongshu-follower-counter/) | Xiaohongshu follower count (Chrome extension + blueprint) |

Full tutorial (deploy a broker → configure the TC002 → send commands with MQTTX → HA blueprints): [Beginner's Guide to MQTT on TC002](https://docs.ulanzistudio.com/tc002/en/software/mqtt.html).

## 5. HTTP Protocol

* **Query endpoints**: `curl http://<IP>/getBase`, `curl http://<IP>/getMqttConfig`; opening `http://<IP>` in a browser gives you the device management page (view / reset). The endpoints are provided by the official firmware and the fields may change between firmware versions.
* **Custom App HTTP protocol (frame pushing)**: built into the official firmware; browser-based tools (such as PixDeck) use it to POST images to the clock. The format is the same as MQTT frame pushing, and PixDeck lets you switch the transport in its settings.
* **Which to choose**: use HTTP for one-off instant pushes; use MQTT for continuous state / automation integration (more reliable, supports retain).

## 6. Agent Setup

To have the TC002 show the run state of an AI coding assistant, the chain is: `Claude Code / Codex hook → Home Assistant entity → blueprint → MQTT → TC002`.

| State          | Light effect   | Scenario             |
| ----------- | ---- | -------------- |
| `idle`      | Green light   | A round of work has finished         |
| `attention` | Flashing yellow light | Running / calling a tool     |
| `blocked`   | Flashing red light | Permission request / failure / blocked |
| `off`       | Lights out   | No task            |

**Minimal setup**: import the [vibe-coding-signal-light blueprint](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fraw.githubusercontent.com%2Fklein0r%2FUlanzi-U-Clock-TC002%2Fdocs%2Ftranslate-to-english%2Fapps%2Fmqtt%2Fvibe-coding-signal-light%2Fblueprint.yaml) into HA → create an `input_select` entity and an automation → a local script (see [AGENT_HOOKS.md](apps/mqtt/vibe-coding-signal-light/docs/AGENT_HOOKS.md) for an example) updates the entity through the HA REST API/webhook → Claude Code / Codex hooks call the script at the right lifecycle points (task submitted → `attention`, failure / permission → `blocked`, finished → `idle`). When several sessions run at once, aggregate with "any blocked → red; any attention → yellow; otherwise lights out".

## 7. Custom Development

1. Install the [FlyThings IDE](https://download.s21i.co99.net/14731609/0/0/ABUIABBPGAAglMLczgYo0Mjk3AU.zip?f=flythings-ide-win32-win32-x86-zkswe-setup.zip&v=1775706403); for the full documentation see [ide-guide/documentation.md](ide-guide/documentation.md)
2. Import `Z21_TC002_Demo/` (`File → Import → Existing Projects into Workspace`)
3. Build: `Ctrl+Alt+Z` (the output goes to `Release/`)
4. Flashing: Wi-Fi ADB only — in the IDE go to `Debug configuration → ADB configuration → WIFI`, enter the IP and choose `Download and debug` (`Ctrl+Alt+R`, not persistent; for a persistent install use `Image build` to produce `update.img`, put it in the root of a FAT32 TF card and upgrade from the card)
5. Restoring the official firmware: power on while holding the reset button next to the USB-C port

**Things to watch out for**: the entry point must call `SystemProperties::setString("sys.zkapp.state","running")` to prevent bricking; the MCU must be initialized before refreshing the LEDs; `sendLedData` needs a frame interval of ≥15 ms; BLE depends on Wi-Fi; do not block the UI main thread; do not modify `src/activity/` (generated automatically by the IDE).

**API quick reference** (sample code for LED / buttons / audio / MIC / Wi-Fi / BLE / GPIO): see [Z21_TC002_Demo/README.md](Z21_TC002_Demo/README.md).

## 8. FAQ

* **Network setup stuck at 100%?** See the [connection troubleshooting guide](https://docs.ulanzistudio.com/tc002/en/faq/).
* **MQTT published but nothing is displayed?** Check the topic (the device's configuration is authoritative for the prefix), whether the clock is currently showing that custom app (updating the content does not switch apps automatically), and whether the payload is UTF-8 JSON.
* **PixDeck does nothing?** Check that you are on the same local network as the clock, that the IP is correct, and that the clock is on a screen that can be updated.

## 9. Community and Contributing

Submit **FlyThings apps** (`apps/flythings/`) or **MQTT apps** (`apps/mqtt/`, guidelines in [apps/mqtt/README.md](apps/mqtt/README.md)). For the full guidelines see [CONTRIBUTING.md](CONTRIBUTING.md).

## 10. License

[GPL-3.0-or-later](LICENSE): derivative works must be distributed under the same license, keep the copyright notices and state the changes made. For third-party components (BlueZ, Adafruit GFX and others) see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
