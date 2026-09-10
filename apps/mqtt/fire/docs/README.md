# TC002 Virtual Fireplace

[![Open your Home Assistant instance and show the blueprint import dialog.](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FUlanziTechnology%2FUlanzi-U-Clock-TC002%2Fblob%2Fmain%2Fapps%2Fmqtt%2Ffire%2Fblueprint.yaml)

## Introduction

Watches a switch/boolean entity: when it turns on, a looping fire is lit on the TC002; when it turns off, the fire goes out (black screen). It works with `input_boolean`, `switch`, or a template switch based on "temperature below threshold" (light a fire when it gets cold).

## Preview

Rendered device output (52×16 pixels):

![demo](../preview/demo.gif)

Photographed on real hardware:

![device](../preview/device.gif)

(`lab/build/fire.gif` is the fire asset generated locally.)

## Configuration Parameters

- **Switch entity** + **"on" state value** (default `on`).
- **TC002 Custom App MQTT topic** (default `ulanzi_1bf6/custom/fire`), **display duration**, **retained message**.

## MQTT

- **Topic**: `[PREFIX]/custom/fire`
- **Payload (on)**: `{"duration":3600,"text":[],"image":[{"data":"data:image/gif;base64,...","position":[0,0]}],"draw":[]}`
- **Payload (off)**: `{"duration":3600,"text":[],"image":[],"draw":[{"df":[0,0,51,15,"#000000"]}]}`

## Assets and License

The fire is original procedurally generated content (`lab/render_fire.py`, Pillow); no third-party assets are used. To regenerate: `python3 lab/render_fire.py`. Both the blueprint and the assets are GPL-3.0-or-later.

## Known Issues

- After receiving an update, the TC002 does not necessarily switch to that app automatically.
