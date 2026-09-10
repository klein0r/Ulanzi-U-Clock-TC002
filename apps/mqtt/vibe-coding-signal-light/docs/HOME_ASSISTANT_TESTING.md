# Home Assistant and TC002 MQTT Real-Device Testing Guide

This document explains how to test `vibe-coding-signal-light` on real TC002 hardware.

Author: 王行知

The shortest chain looks like this:

```text
Home Assistant -> MQTT broker -> TC002
```

An MQTT broker can be thought of as a message relay station. Home Assistant sends messages to the broker, the TC002 connects to the same broker, and receives the content to display from the corresponding topic.

## 1. Basic Concepts

### MQTT broker

An MQTT broker is a lightweight message server.

In this app it receives the TC002 Custom App payload published by Home Assistant and forwards it to the TC002.

Common choices:

- The Mosquitto broker Home Assistant add-on
- Mosquitto running on a Mac, a NAS or a server
- Any other MQTT broker you already use at home

### Topic

A topic is an MQTT message address.

The TC002 Custom App MQTT topic is usually:

```text
[PREFIX]/custom/[APP_NAME]
```

Example:

```text
ulanzi_1bf6/custom/vibe_signal
```

`[PREFIX]` is usually made up of the MQTT prefix and the last four digits of the device's MAC address:

```text
mqtt_prefix + "_" + last 4 digits of MAC
```

If the MQTT prefix is `ulanzi` and the last four digits of the device MAC are `1bf6`, the prefix is usually:

```text
ulanzi_1bf6
```

### Payload

The payload is the JSON content the TC002 actually displays.

This blueprint publishes a Custom App payload containing a small PNG/GIF image. The image is placed directly into the MQTT message as a base64 data URL, so Home Assistant only has to publish a single message.

## 2. Recommended Testing Order

Do not start by testing the complete Home Assistant automation. Testing layer by layer is recommended:

1. Start an MQTT broker.
2. Confirm that your machine can publish to and subscribe from the broker.
3. Point the TC002 at the same broker.
4. Publish a yellow light payload manually first.
5. Then import the Home Assistant blueprint.
6. Finally, switch the light effects using the Home Assistant state entity.

That way you can troubleshoot the broker, the topic, the TC002 configuration and the Home Assistant automation separately.

## 3. Setting Up a Broker with Mosquitto on a Mac

Install Mosquitto:

```bash
brew install mosquitto
```

Start the background service:

```bash
brew services start mosquitto
```

Check that the command-line tools are available:

```bash
mosquitto -h
mosquitto_pub --help
mosquitto_sub --help
```

Find the Mac's LAN IP:

```bash
ipconfig getifaddr en0
```

Example:

```text
10.19.1.58
```

The TC002 must be able to reach port `1883` on that IP.

## 4. Local Broker Self-Test

Subscribe to messages in one terminal:

```bash
mosquitto_sub -h 127.0.0.1 -t tc002/test
```

Publish a message in another terminal:

```bash
mosquitto_pub -h 127.0.0.1 -t tc002/test -m "hello tc002"
```

If the subscribing window prints `hello tc002`, the broker sends and receives correctly on the local machine.

## 5. Configure MQTT on the TC002

In the TC002's MQTT settings, enter:

```text
Broker host: <Mac LAN IP>
Broker port: 1883
Username: can be left empty for local testing, unless the broker requires authentication
Password: can be left empty for local testing, unless the broker requires authentication
MQTT prefix: ulanzi
```

You can also check the TC002's current network and MQTT configuration through the HTTP interface:

```bash
curl http://<TC002_IP>/getBase
curl http://<TC002_IP>/getMqttConfig
```

If the TC002 and the broker are not on the same Wi-Fi or local network, the device will not receive any messages.

## 6. Manual Publish Test

First work out the topic.

Example:

- MQTT prefix: `ulanzi`
- Last 4 digits of the TC002 MAC: `1bf6`
- Custom app name: `vibe_signal`

The topic is:

```text
ulanzi_1bf6/custom/vibe_signal
```

Publish a simple yellow light payload first, to verify the chain quickly:

```bash
mosquitto_pub -h 127.0.0.1 -t ulanzi_1bf6/custom/vibe_signal -m '{"duration":3600,"text":[],"image":[],"draw":[{"df":[0,0,52,16,"#000000"]},{"dfc":[26,8,5,"#FFCB52"]}]}'
```

Expected result:

- The TC002 receives the Custom App update.
- If the device is currently showing `vibe_signal`, the screen turns yellow.

### Verified Results

The following environment was verified successfully on 2026-06-23:

```text
TC002 IP: 10.19.1.128
TC002 MAC: ccc4b2441bf6
MQTT broker: 10.19.1.58:1883
MQTT prefix: ulanzi
Topic: ulanzi_1bf6/custom/vibe_signal
```

After publishing the yellow light Custom App payload to the topic via Mosquitto, the real TC002 screen successfully turned yellow.

Once the basic chain is confirmed, test the final result with the image payload from the blueprint or with a Home Assistant automation. The image version uses traffic light assets on a pure black background and looks better than the simple `draw` probe.

Note:

After a Custom App's content is updated, the TC002 does not necessarily switch to that app automatically. If the screen does not change, switch to the target custom app manually on the device first, or publish the message to the name of the app currently being displayed.

## 7. Home Assistant Setup

### Option A: Use an Existing Home Assistant

If Home Assistant already has the MQTT integration configured, just use the existing MQTT integration.

If you do not have MQTT yet:

1. If your Home Assistant installation supports add-ons, install the Mosquitto broker add-on.
2. Create an MQTT username and password.
3. Add the MQTT integration.
4. Point the TC002 at the same broker.

### Option B: Mac Broker + Existing Home Assistant

You can also use the Mac as the broker and have both Home Assistant and the TC002 connect to the Mac's LAN IP.

Home Assistant MQTT broker settings:

```text
Broker: <Mac LAN IP>
Port: 1883
Username/password: can be left empty for local testing, unless the broker requires authentication
```

The TC002's MQTT broker settings must use the same host and port.

## 8. Create the Home Assistant State Entity

For a first test it is recommended to create an `input_select` helper with the following options:

```text
off
idle
working
attention
blocked
```

This helper is the blueprint's "state entity".

## 9. Import the Blueprint

Import:

```text
apps/mqtt/vibe-coding-signal-light/blueprint.yaml
```

Then create an automation from it.

Recommended configuration:

```text
State entity: the input_select helper created in the previous step
TC002 Custom App MQTT topic: ulanzi_1bf6/custom/vibe_signal
Display duration: 3600
Retain MQTT message: false
```

Then switch the helper's state:

```text
attention -> yellow light
blocked -> red light
idle -> green light
working -> green, yellow, red in a loop
off -> all black, lights out
```

## 10. Integrating Claude Code / Codex

Once you have confirmed that the Home Assistant helper drives the TC002, you can hook up your coding tools.

The basic idea:

```text
Claude Code / Codex hook -> updates the input_select -> the blueprint publishes over MQTT -> the TC002 displays it
```

It is recommended to test manually with the Home Assistant API first:

```bash
curl -X POST "http://<HA_HOST>:8123/api/services/input_select/select_option" \
  -H "Authorization: Bearer <HA_LONG_LIVED_ACCESS_TOKEN>" \
  -H "Content-Type: application/json" \
  -d '{"entity_id":"input_select.tc002_vibe_status","option":"attention"}'
```

If the TC002 turns yellow, the code agent hook only needs to run a command like this at the right moments.

Suggested mapping:

| Tool event | State |
|---|---|
| User submits a task, execution starts, a tool is being called | `attention` |
| Permission request, failure, blocked | `blocked` |
| Task finished normally | `idle`, then `off` a few seconds later |

For a more complete guide, see `AGENT_HOOKS.md`.

### The Verified Webhook Chain

The following chain has been verified in local testing:

```text
Claude Code hook -> Home Assistant webhook -> mqtt.publish -> Mosquitto -> TC002
```

Key points:

- Home Assistant needs `webhook:` explicitly enabled.
- The webhook automation uses `local_only: true` and only accepts requests from the local machine, which suits debugging Claude Code / Codex on the same machine.
- It is recommended to append `>/dev/null 2>&1 || true` to Claude Code hook commands, so that a failing traffic light script does not affect Claude Code starting or running normally.
- Using `SessionStart` as a traffic light hook is not recommended. If a hook fails during the startup phase, Claude Code may fail to start.

## 11. Troubleshooting

### MQTT publishes successfully but nothing changes on the TC002

Check:

- Whether the TC002 and the broker are on the same Wi-Fi or local network.
- Whether the TC002's MQTT broker host is correct.
- Whether the topic prefix matches the last four digits of the device's MAC address.
- Whether the app name matches the custom app currently displayed on the TC002.
- Whether port `1883` on the broker is reachable.

### The Home Assistant automation ran but the broker received no message

Subscribe to the target topic:

```bash
mosquitto_sub -h <BROKER_HOST> -t 'ulanzi_1bf6/custom/vibe_signal' -v
```

Then switch the Home Assistant helper's state.

If no message appears in the subscribing window, the problem is usually in the Home Assistant automation or the MQTT integration.

### The broker works locally but the TC002 receives nothing

Check the macOS firewall, router AP isolation, guest network isolation and similar. Some Wi-Fi networks block devices on the LAN from reaching each other.

### The colors are wrong

Make sure the state entity's state text matches exactly one of the following values:

```text
off
idle
working
attention
blocked
```

If you changed the state value configuration in the blueprint, make sure the entity's output was changed accordingly.

### Flicker when filming with a phone

The TC002 is an LED matrix display and normally shows the image using row/column scanning and PWM dimming. When filming with an iPhone, the sensor's rolling shutter, the video frame rate and the exposure time may not be in sync with the LED refresh rhythm, which makes flicker, brightness jumps or horizontal dark bands appear in the video.

This is common when filming LED screens, car lights, displays and stage lighting, and does not necessarily mean the image flickers to the naked eye. Things you can try:

- Switch between 30fps / 60fps in the iPhone camera.
- Turn off auto exposure and lower the exposure manually.
- Change the shooting distance and angle.
- Increase the ambient light so the camera does not raise the shutter speed or ISO automatically.
- If the firmware or the device settings support it, try adjusting the screen brightness.

## 12. Pre-Contribution Checklist

- `blueprint.yaml` can be imported by Home Assistant.
- `docs/README.md` documents the dependencies, installation, configuration and topic.
- `preview/demo.gif` shows the actual light effects.
- At least one test on real hardware or at the broker level has been completed, with the environment recorded.

## 13. Simulating the Three Code Agent States

After integrating the Home Assistant webhook or helper, it is recommended to simulate at least three real usage states:

| Simulated scenario | State value | Expected display |
|---|---|---|
| Code agent is running / calling a tool | `attention` | Flashing yellow light |
| User input / permission confirmation needed, or an error | `blocked` | Flashing red light |
| A round of work has completed | `idle`, then `off` | Green light briefly, then all black |

If you use the webhook chain, you can subscribe to the MQTT topic to watch the messages:

```bash
mosquitto_sub -h <BROKER_HOST> -t 'ulanzi_1bf6/custom/vibe_signal' -v
```

Then have the hook send, in order:

```text
PreToolUse or UserPromptSubmit -> attention
PermissionRequest or Error     -> blocked
Stop or Done                   -> idle, then off a few seconds later
```

In this project's own testing, `Claude Code hook -> Home Assistant webhook -> mqtt.publish -> Mosquitto -> TC002` triggered the following sequence:

```text
flashing yellow -> flashing red -> green -> all black
```
