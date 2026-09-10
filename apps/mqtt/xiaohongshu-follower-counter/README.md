# TC002 Xiaohongshu Follower Count (Chrome + Home Assistant)

The Chrome extension uses the current browser login session to read the follower count already displayed on a given Xiaohongshu profile page, generates a 52×16 PNG with a fixed pixel matrix inside the browser, and then publishes it to one or more TC002 devices through a Home Assistant webhook and the MQTT integration. There is no third-party scraping cloud service, and cookies are never read or exported.

```text
Xiaohongshu profile → Chrome extension → local-only HA webhook → HA MQTT integration → TC002 Custom App
```

## Prerequisites

You must use the **Google Chrome browser** and be logged in to your Xiaohongshu account in Chrome beforehand. The extension reuses the current browser login session to read the follower count already displayed on the user's profile page; it does not log in on the user's behalf, and it does not read or upload cookies.

## Installation

- [Quick install](docs/QUICKSTART.md)
- [Full configuration, privacy and troubleshooting](docs/README.md)
- [Chrome extension ZIP](release/xiaohongshu-follower-counter-chrome-0.2.0.zip)
- [SHA-256](release/SHA256SUMS)
- [Home Assistant blueprint](blueprint.yaml)

[![Open your Home Assistant instance and show the blueprint import dialog](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FUlanziTechnology%2FUlanzi-U-Clock-TC002%2Fblob%2Fmain%2Fapps%2Fmqtt%2Fxiaohongshu-follower-counter%2Fblueprint.yaml)

A single blueprint instance can serve multiple devices. Each TC002 is bound to one Xiaohongshu profile in the extension; the blueprint only permits device prefixes on an explicit allowlist, and fixes the app name after `custom/`. The refresh interval has a minimum and a default of 5 minutes (300 seconds).

## Real-World Results

Verified on Chrome, Home Assistant 2026.6.3 and real TC002 hardware: the extension binds different Xiaohongshu profiles to the corresponding devices, and after the blueprint validates the device prefix, the 52×16 pixel image is published over the MQTT Custom App topic.

| Home Assistant blueprint | Chrome extension configuration |
| --- | --- |
| ![Home Assistant blueprint configuration](resources/home-assistant-blueprint.png) | ![Chrome extension configuration](resources/chrome-extension-options.png) |

![TC002 hardware showing the Xiaohongshu follower count](resources/tc002-device.jpg)

- Render preview: [preview/demo.png](preview/demo.png)
- Real-device material requirements: [preview/README.md](preview/README.md)
License: GPL-3.0-or-later.
