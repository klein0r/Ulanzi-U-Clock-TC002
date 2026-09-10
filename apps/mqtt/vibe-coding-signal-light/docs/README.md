# TC002 Vibe Coding Traffic Light

[![Import into Home Assistant](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fraw.githubusercontent.com%2Fklein0r%2FUlanzi-U-Clock-TC002%2Fdocs%2Ftranslate-to-english%2Fapps%2Fmqtt%2Fvibe-coding-signal-light%2Fblueprint.yaml)

## Introduction

This app turns the Ulanzi U-Clock TC002 into a desktop status traffic light, suitable for showing the working state of coding assistants, CI jobs, local scripts, automation flows and so on.

Author: 王行知

At its core is a Home Assistant blueprint. The blueprint watches the state changes of a Home Assistant entity and then publishes the content to be displayed to the TC002's Custom App topic over MQTT.

What this app is really aimed at is the vibe coding scenario: tools like Claude Code, Codex, CI and local scripts write their state into Home Assistant when they start running, call a tool, hit a permission prompt or an error, or finish; Home Assistant then drives the TC002 traffic light over MQTT.

The display assets are currently 52x16 PNG/GIF images; being small, they are embedded directly in `blueprint.yaml`. The background pixels outside the lamps are pure black, and the horizontal spacing between the three lamps is uniform.

To reduce dark-area flicker when filming the LED matrix with a phone, unlit lamps do not keep any dark red, dark yellow or dark green pixels but are completely black; only the currently lit lamp is drawn.

Supported states:

| State value | Display |
|---|---|
| `off` | All black, lights out |
| `idle` | Green light |
| `working` | Green, yellow, red in a loop |
| `attention` | Flashing yellow light |
| `blocked` | Flashing red light |

## Preview

See `preview/demo.gif`.

## Dependencies

- Home Assistant with the MQTT integration enabled
- An MQTT broker reachable from both Home Assistant and the TC002
- A TC002 connected to that same MQTT broker
- A custom app on the TC002 whose name matches the `[APP_NAME]` in the MQTT topic

## Installation

1. Import `blueprint.yaml` into Home Assistant.
2. Create or pick an entity to hold the state. For a first test, an `input_select` helper is recommended.
3. Create an automation from this blueprint.
4. Set `TC002 Custom App MQTT topic` to your device's topic.

Topic format:

```text
[PREFIX]/custom/[APP_NAME]
```

Example:

```text
ulanzi_1bf6/custom/vibe_signal
```

`[PREFIX]` is usually the MQTT prefix plus the last four digits of the device's MAC address. For example, with the MQTT prefix `ulanzi` and a device MAC ending in `1bf6`, the prefix is usually `ulanzi_1bf6`.

## Using It with Claude Code / Codex

The recommended chain:

```text
Claude Code / Codex hook
  -> updates the Home Assistant state entity
  -> the blueprint publishes over MQTT
  -> the TC002 shows the traffic light
```

For a first test you can switch the `input_select` helper by hand. Once the TC002 displays correctly, hook up Claude Code, Codex or another coding assistant.

Suggested event mapping:

| Code agent behavior | Suggested state value | Light effect |
|---|---|---|
| No task running | `off` | All black, lights out |
| A round of work finished normally | `idle` | Green light |
| User submitted a task, agent is running, a tool is being called | `attention` | Flashing yellow light |
| Permission confirmation, command failure, test failure, blocked flow | `blocked` | Flashing red light |
| Manual animation test | `working` | Green, yellow, red in a loop |

If you use the Home Assistant REST API, your hook can update the helper with the following command:

```bash
curl -X POST "http://<HA_HOST>:8123/api/services/input_select/select_option" \
  -H "Authorization: Bearer <HA_LONG_LIVED_ACCESS_TOKEN>" \
  -H "Content-Type: application/json" \
  -d '{"entity_id":"input_select.tc002_vibe_status","option":"attention"}'
```

Change the `option` at the end to `idle`, `blocked` or `off` to drive the different light effects.

Claude Code, Codex or any other tool that supports hooks / shell commands can invoke this command at the corresponding points in its lifecycle:

| Hook scenario | Suggested command |
|---|---|
| User submits a task / execution starts | Set to `attention` |
| Before and after tool calls / while a command runs | Set to `attention` |
| Permission request / failure / blocked | Set to `blocked` |
| A round of work finishes | Set to `idle`, and optionally to `off` a few seconds later |

If you would rather not put a complex `curl` into every hook, you can wrap it in your own script, e.g. `tc002-vibe-status.sh attention`, which then calls the Home Assistant API internally. That way Claude Code, Codex and CI can all reuse the same status exit point.

For a more detailed integration guide, see `AGENT_HOOKS.md`.

## Configuration Options

| Option | Description |
|---|---|
| State entity | The Home Assistant entity that drives the traffic light |
| TC002 Custom App MQTT topic | The target topic, usually `[PREFIX]/custom/[APP_NAME]` |
| Display duration | The `duration` value written into the TC002 Custom App payload |
| Retain MQTT message | Whether the broker should retain the last message, so the display is restored after the device reconnects |
| State values | The state text corresponding to each light effect |

## MQTT Payload

The blueprint publishes a TC002 Custom App JSON payload. The image is sent through the `image` field as an inline base64 data URL, so there is no need to upload the image separately or to use an external image host.

Example structure:

```json
{
  "duration": 3600,
  "text": [],
  "image": [
    {
      "data": "data:image/gif;base64,...",
      "position": [0, 0]
    }
  ],
  "draw": []
}
```

## Testing on Real Hardware

For detailed steps see `HOME_ASSISTANT_TESTING.md`.

Quick checklist:

1. Check the TC002's current IP and MQTT configuration:

   ```bash
   curl http://<TC002_IP>/getBase
   curl http://<TC002_IP>/getMqttConfig
   ```

2. Make sure the TC002 and Home Assistant use the same MQTT broker.

3. Determine the topic. For example, with the MQTT prefix `ulanzi`, a device MAC ending in `1bf6` and the custom app named `vibe_signal`, the topic is:

   ```text
   ulanzi_1bf6/custom/vibe_signal
   ```

4. Make sure the TC002 is currently showing the corresponding custom app, e.g. `vibe_signal`. Updating a custom app's content does not necessarily switch to that app automatically.

5. In Home Assistant, switch the state entity to `attention`, `blocked`, `idle` and `off` one after another.

6. The TC002 should show a yellow light, a red light, a green light and then go dark.

You can verify the underlying MQTT chain quickly with a `draw` payload first:

```bash
mosquitto_pub -h <BROKER_HOST> -t ulanzi_1bf6/custom/vibe_signal -m '{"duration":3600,"text":[],"image":[],"draw":[{"df":[0,0,52,16,"#000000"]},{"dfc":[26,8,5,"#FFCB52"]}]}'
```

## Known Issues

- The current version embeds the small images directly in `blueprint.yaml`, which makes installation as simple as possible, but means the base64 has to be updated whenever the graphics change.
- After receiving a Custom App update, the TC002 does not necessarily switch to that app automatically. When testing, it is best to switch to the target custom app manually on the device first, or publish to the name of the app currently being displayed.
- When recording the TC002 with a phone camera, the video may show flicker or horizontal dark bands rolling across the screen. This is usually caused by the LED matrix scan refresh / PWM dimming not being in sync with the camera shutter, and does not necessarily mean the image flickers to the naked eye.
- The TC002's MQTT topic rules may change with official firmware updates, so the topic has been made configurable.

## License

GPL-3.0-or-later.
