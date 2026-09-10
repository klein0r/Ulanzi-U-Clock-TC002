# TC002 Desktop Pixel Pet (Grey Cat)

[![Open your Home Assistant instance and show the blueprint import dialog.](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FUlanziTechnology%2FUlanzi-U-Clock-TC002%2Fblob%2Fmain%2Fapps%2Fmqtt%2Fpet%2Fblueprint.yaml)

## Introduction

Turns the TC002 into a desktop pixel pet (a light grey tabby cat). The blueprint watches a HA entity and switches actions according to its state:

| State value (configurable) | Action |
|---|---|
| `idle` | Standing + slight breathing |
| `walk` | Walking (stepping sideways) |
| `run` | Running |

The cat is displayed centered on the 52×16 screen.

## Preview

Rendered device output (52×16 pixels):

![demo](../preview/demo.gif)

Photographed on real hardware:

![device](../preview/device.gif)

## Configuration Parameters

- **State entity**: the HA entity that provides the current state (e.g. `input_select.pet`).
- **TC002 Custom App MQTT topic**: `[PREFIX]/custom/[APP_NAME]`, default `ulanzi_1bf6/custom/pet`.
- **Display duration / retained message / idle·walk·run state values**.

## MQTT

- **Topic**: `[PREFIX]/custom/pet` (example: `ulanzi_1bf6/custom/pet`)
- **Payload** (switches the embedded GIF according to the state):

```json
{"duration": 3600, "text": [], "image": [{"data": "data:image/gif;base64,...", "position": [0, 0]}], "draw": []}
```

## Assets and License

- Cat assets: **Shepardskin — "Cat Sprites" (CC0 / public domain)**, from <https://opengameart.org/content/cat-sprites>, recolored to a light grey tabby. CC0 is compatible with this repository's GPL-3.0.
- To regenerate the assets: `python3 lab/build_pet.py` (requires Pillow).
- The blueprint itself: GPL-3.0-or-later.

## Known Issues

- After receiving a Custom App update, the TC002 does not necessarily switch to that app automatically.
- Device fonts/rendering are device-side behavior; the payload frame format is identical to the HTTP Custom App.
