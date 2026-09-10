# Claude Code / Codex Integration Guide

This traffic light is not just a Home Assistant toy; its main use case is vibe coding: while Claude Code, Codex, CI or a local script is running, the TC002 can indicate the current state like a desktop status light.

Author: 王行知

## Recommended Architecture

```text
Code agent hook
  -> Home Assistant state entity
  -> Blueprint
  -> MQTT broker
  -> TC002 Custom App
```

The code agent here can be Claude Code, Codex or any other tool capable of running a shell command.

## State Conventions

| State value | Light effect | Suitable for |
|---|---|---|
| `off` | All black, lights out | No task running |
| `idle` | Green light | A round of work finished normally, a brief success indication |
| `attention` | Flashing yellow light | The agent is running, calling a tool, or needs attention |
| `blocked` | Flashing red light | Permission request, command failure, test failure, blocked flow |
| `working` | Green, yellow, red in a loop | Manual animation testing; not recommended for long-term use in automatic hooks |

## Home Assistant Helper

It is recommended to create an `input_select` helper, for example:

```text
input_select.tc002_vibe_status
```

Options:

```text
off
idle
working
attention
blocked
```

The blueprint watches this entity and sends the corresponding light effect to the TC002 whenever the state changes.

## Updating the State via the Home Assistant API

After creating a Home Assistant long-lived access token, you can update the helper through the REST API:

```bash
curl -X POST "http://<HA_HOST>:8123/api/services/input_select/select_option" \
  -H "Authorization: Bearer <HA_LONG_LIVED_ACCESS_TOKEN>" \
  -H "Content-Type: application/json" \
  -d '{"entity_id":"input_select.tc002_vibe_status","option":"attention"}'
```

Just replace `option` with another state value.

## Recommended: Use a Home Assistant Webhook

If you would rather not put a Home Assistant token into your Claude Code / Codex hooks, a Home Assistant webhook is recommended. The hook only sends a state or a complete TC002 payload to the local HA webhook, and a Home Assistant automation then performs the `mqtt.publish`.

The chain:

```text
Claude Code / Codex hook
  -> http://127.0.0.1:8125/api/webhook/tc002_vibe_signal_hook
  -> Home Assistant automation
  -> mqtt.publish
  -> TC002
```

Home Assistant needs `webhook:` enabled and an automation such as:

```yaml
- id: tc002_vibe_signal_webhook_to_mqtt
  alias: TC002 Vibe Signal Webhook to MQTT
  triggers:
    - platform: webhook
      webhook_id: tc002_vibe_signal_hook
      allowed_methods:
        - POST
      local_only: true
  actions:
    - service: mqtt.publish
      data:
        topic: "{{ trigger.json.topic | default('ulanzi_1bf6/custom/vibe_signal') }}"
        payload: "{{ trigger.json.payload | to_json }}"
        qos: 0
        retain: false
  mode: queued
```

Local test:

```bash
curl -X POST "http://127.0.0.1:8125/api/webhook/tc002_vibe_signal_hook" \
  -H "Content-Type: application/json" \
  -d '{"topic":"ulanzi_1bf6/custom/vibe_signal","payload":{"duration":3,"text":[],"image":[],"draw":[{"df":[0,0,52,16,"#000000"]},{"dfc":[25,8,5,"#FFCB52"]}]}}'
```

If the TC002 turns yellow, the `HA webhook -> MQTT -> TC002` part of the chain works. After that, the Claude Code / Codex hook only needs to send the corresponding state or payload to this webhook.

## Wrapping It in a Script

You can create a local script, for example `tc002-vibe-status.sh`:

```bash
#!/usr/bin/env bash
set -euo pipefail

STATUS="${1:-attention}"
HA_HOST="${HA_HOST:-http://127.0.0.1:8123}"
HA_ENTITY="${HA_ENTITY:-input_select.tc002_vibe_status}"

curl -sS -X POST "$HA_HOST/api/services/input_select/select_option" \
  -H "Authorization: Bearer $HA_TOKEN" \
  -H "Content-Type: application/json" \
  -d "{\"entity_id\":\"$HA_ENTITY\",\"option\":\"$STATUS\"}" >/dev/null
```

Set the following before using it:

```bash
export HA_HOST="http://127.0.0.1:8123"
export HA_TOKEN="your Home Assistant long-lived access token"
export HA_ENTITY="input_select.tc002_vibe_status"
```

Manual test:

```bash
./tc002-vibe-status.sh attention
./tc002-vibe-status.sh blocked
./tc002-vibe-status.sh idle
./tc002-vibe-status.sh off
```

## Claude Code Integration

Claude Code can run hooks at key points in its lifecycle. The configuration format may change between versions, so follow your current Claude Code documentation, but the mapping can stay the same:

| Claude Code scenario | Suggested state |
|---|---|
| User submits a task | `attention` |
| Before a tool call / while a command runs | `attention` |
| Permission request | `blocked` |
| Tool failure / test failure | `blocked` |
| A round of work finishes | `idle`, then `off` a few seconds later |

Example hook commands:

```bash
/path/to/tc002-vibe-status.sh attention
/path/to/tc002-vibe-status.sh blocked
/path/to/tc002-vibe-status.sh idle
```

## Codex Integration

Codex, or any other coding tool that supports hooks / shell commands, can use the same script.

Suggested mapping:

| Codex scenario | Suggested state |
|---|---|
| Starting to handle a user request | `attention` |
| Running a tool or command, or editing files | `attention` |
| User confirmation needed, permission denied, execution failed | `blocked` |
| Current task completed | `idle`, then `off` a few seconds later |

## When Several Tools Run at Once

If you often have several Claude Code / Codex sessions open at the same time, it is better not to let each hook simply "turn the light off when it finishes". A more robust approach is to keep a local state file recording the currently active sessions:

```text
session A: attention
session B: blocked
session C: done
```

Suggested aggregation rules:

```text
any session = blocked   -> red light
any session = attention -> yellow light
no active session        -> brief green indication, then lights out
```

That way, when one task finishes it does not accidentally turn off the light for another task that is still running.

## Minimal Working Setup

If you just want to get it running without complex state aggregation:

1. When Claude Code / Codex starts running, execute `tc002-vibe-status.sh attention`.
2. On an error or a permission prompt, execute `tc002-vibe-status.sh blocked`.
3. On a normal finish, execute `tc002-vibe-status.sh idle`.
4. A few seconds later, execute `tc002-vibe-status.sh off`.

Once you have confirmed that it works reliably on real hardware, add the multi-session aggregation logic.
