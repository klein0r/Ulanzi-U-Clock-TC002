# TC002 Now Playing (Marquee)

> **This is a "script-based" MQTT app, not a one-click HA blueprint import.** The reason is explained below.

## Introduction

Shows "title - artist" as a **horizontally scrolling marquee** on the TC002, with a play/pause icon on the left. Publish once whenever the song changes and the device keeps scrolling in a loop until the next track.

**Why not a blueprint?** The TC002 Custom App `text` element **cannot scroll** (anything beyond the edge is simply clipped), so "text scrolling continuously to the left" can only be achieved by **baking the scrolling into a multi-frame GIF** and letting the device loop it. The GIF has to be rendered in Python, which HA blueprint templates cannot do — hence this app is a small **publisher script** (the official repository explicitly accepts "non-blueprint MQTT integration approaches").

## Preview

Rendered device output (52×16 pixels, marquee scrolling):

![demo](../preview/demo.gif)

Photographed on real hardware:

![device](../preview/device.gif)

## Dependencies

- Python 3 + Pillow (`pip install pillow`)
- An MQTT broker reachable from both the TC002 and the machine running the script (not needed for the `--http` preview path)

## Usage

```bash
# Publish over MQTT (the normal usage)
python3 lab/nowplaying_publisher.py --broker 192.168.1.5 --prefix ulanzi_1bf6 \
    --title "Midnight City" --artist "M83" --state playing

# Push straight to the device over HTTP for a preview (no broker needed; for recording previews / debugging)
python3 lab/nowplaying_publisher.py --title "Midnight City" --artist "M83" --http 192.168.1.50

# Only render to a GIF file
python3 lab/nowplaying_publisher.py --title "Midnight City" --artist "M83" --dry demo.gif
```

Short titles are shown statically centered instead of scrolling pointlessly; only long titles scroll.

## Home Assistant Integration

Wrap the script in a `shell_command`, then call it from an automation whenever the `media_player` changes:

```yaml
# configuration.yaml
shell_command:
  tc002_nowplaying: >-
    python3 /config/tc002/nowplaying_publisher.py
    --broker 192.168.1.5 --prefix ulanzi_1bf6
    --title "{{ title }}" --artist "{{ artist }}" --state "{{ st }}"
```

```yaml
# Automation
- alias: TC002 now playing
  trigger:
    - platform: state
      entity_id: media_player.spotify
  action:
    - service: shell_command.tc002_nowplaying
      data:
        title: "{{ state_attr('media_player.spotify','media_title') | default('') }}"
        artist: "{{ state_attr('media_player.spotify','media_artist') | default('') }}"
        st: "{{ 'playing' if states('media_player.spotify') == 'playing' else 'paused' }}"
```

## MQTT

- **Topic**: `[PREFIX]/custom/nowplaying` (example: `ulanzi_1bf6/custom/nowplaying`)
- **Payload** (a single looping GIF, `retain=true`):

```json
{"duration": 3600, "text": [], "image": [{"data": "data:image/gif;base64,...", "position": [0, 0]}], "draw": []}
```

## Known Issues / Limitations

- The device font is ASCII only: non-ASCII characters (Chinese and others) are filtered out.
- The icon and the text are original pixel art (generated inside the script); no third-party assets are used.
- The marquee is a single GIF: it only needs to be republished when the title changes (the script publishes once and the device loops forever).

## License

GPL-3.0-or-later (script and generated assets).
