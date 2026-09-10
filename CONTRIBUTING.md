# Contributing Guide

Thank you for your interest in the **Ulanzi U-Clock TC002** open source project! This document explains how to contribute your own code to this repository.

This repository currently accepts **two types** of app contributions:

| Type | Description | Submission directory |
|---|---|---|
| **FlyThings app** | A FlyThings IDE project that runs directly on the TC002 device, built and flashed onto the device | `apps/flythings/<your-app-name>/` |
| **MQTT app** | Mostly **Home Assistant blueprints** that users import into their own HA instance with one click and use to interact with the TC002 over MQTT. Other MQTT-based integration approaches such as Node-RED flows and openHAB rules are also accepted | `apps/mqtt/<your-app-name>/` |

> Use lowercase letters + digits + hyphens for the app name `<your-app-name>`, for example `weather-clock`, `stock-ticker`, `battery-monitor`. Names must be unique within a type.
>
> Once an app passes review and is merged, it is added to the **community app list in the top-level [`README.md`](README.md)** (FlyThings apps under "What Is Open Source Here", MQTT apps under the "MQTT Apps" section), as one of the entry points for TC002 users.
>


---

## General Requirements (Mandatory for Both Types)

### 1. License Compatibility

The main body of this repository uses **[GPL-3.0-or-later](LICENSE)**, so the app you submit must use **one of the following licenses**:

- GPL-3.0-or-later (recommended)
- GPL-2.0-or-later
- LGPL-2.1-or-later, LGPL-3.0-or-later
- Apache-2.0
- MIT, BSD-2-Clause, BSD-3-Clause
- Any other [GPL-3.0 compatible license](https://www.gnu.org/licenses/license-list.html#GPLCompatibleLicenses)

How to declare the license:

- **FlyThings apps**: put a `LICENSE` file in the app directory, or state it clearly at the top of `README.md`
- **MQTT apps**: add a `# SPDX-License-Identifier: <SPDX-ID>` comment line at the top of `blueprint.yaml`; if it differs from the repository's main license, also put the full license text in `docs/LICENSE`

### 2. Required Files

The required files differ slightly between the types:

A **FlyThings app** must contain at least the following at its top level:

```raw
apps/flythings/<your-app-name>/
├── README.md          # App introduction, build/run instructions, screenshots
├── manifest.json      # App metadata (see below)
└── LICENSE            # The full text of a GPL-3.0 compatible license
```

An **MQTT app** (HA blueprint) must contain at least the following at its top level:

```raw
apps/mqtt/<your-app-name>/
├── blueprint.yaml     # The main HA blueprint file; metadata goes directly into the blueprint: section
├── preview/           # At least one screenshot or GIF of it running
└── docs/              # Contains the README + the license declaration
```

> Unlike FlyThings apps, MQTT apps do not require a separate `manifest.json` / `LICENSE` / top-level `README.md`; metadata and the license declaration are carried by the comment header and the `blueprint:` section of `blueprint.yaml`, with the documentation laid out under `docs/`. See "MQTT App Guidelines" below for details.

### 3. `manifest.json` Format (FlyThings Apps Only)

```json
{
  "name": "weather-clock",
  "displayName": "天气时钟",
  "version": "1.0.0",
  "type": "flythings",
  "author": "Your Name <your@email.com>",
  "license": "GPL-3.0-or-later",
  "description": "Shows the current weather and a three-day forecast on the home screen.",
  "tags": ["weather", "clock", "home"],
  "minFirmware": "1.0.0",
  "homepage": "https://github.com/your-user/your-repo"
}
```

Field descriptions:

| Field | Required | Description |
|---|:-:|---|
| `name` | ✅ | Must match the directory name |
| `displayName` | ✅ | The name shown in the UI (Chinese is supported) |
| `version` | ✅ | Semantic version |
| `type` | ✅ | Always `flythings` |
| `author` | ✅ | Author credit + contact details |
| `license` | ✅ | SPDX identifier; must be GPL-3.0 compatible |
| `description` | ✅ | A one-line description of what it does |
| `tags` | ⬜ | Category tags, to make it easier to find |
| `minFirmware` | ⬜ | The minimum firmware version required |
| `homepage` | ⬜ | A link to your own project page / documentation |

### 4. README Template Requirements

Every app's `README.md` must contain at least the following sections:

- **Introduction** — what the app does and which need it addresses
- **Screenshots or video** — how it looks on real hardware (at least one still image; a GIF is recommended for animations)
- **Dependencies** — the third-party libraries, firmware versions and external APIs required
- **Installation and running** — build/deployment steps that let someone else run it as-is
- **Configuration** — if there are configuration files or environment variables, describe each of them
- **Known issues** — unavoidable limitations or open to-dos

### 5. What Is Not Accepted

- Closed-source binaries (unless they are clearly declared third-party dependencies)
- Code containing malicious behavior, privacy theft or unauthorized access
- Infringing resources (unlicensed images, fonts, audio)
- "Filler" projects unrelated to the TC002

---

## FlyThings App Guidelines

> For a complete reference example: [`Z21_TC002_Demo/`](Z21_TC002_Demo/)

### Directory Structure

```raw
apps/flythings/<your-app-name>/
├── README.md
├── manifest.json
├── LICENSE
├── Manifest.xml          # The FlyThings project manifest
├── ui/                   # *.ftu interface files
├── src/
│   ├── Main.cpp          # Entry point
│   ├── activity/         # Generated automatically by the IDE, do not edit by hand
│   ├── logic/            # UI event logic
│   ├── managers/         # Custom managers (optional)
│   ├── pages/            # Custom pages (optional)
│   └── utils/            # Utility classes (optional)
└── resources/            # Resources packaged with the firmware (images, audio, etc.)
```

### Requirements Before Submitting

1. **It builds without errors in the FlyThings IDE** and runs successfully on real hardware or in TF card boot mode. For real-device verification use Wi-Fi ADB `Download and debug` (`Ctrl+Alt+R`, not persistent), or `Image build` to produce `update.img`, put it in the root of a FAT32 TF card and upgrade from the card — for the exact steps see the [Custom Development](README.md#7-custom-development) section of the top-level [`README.md`](README.md)
2. **Do not commit `Release/` build output** (add `Release/` to the `.gitignore` in your subdirectory)
3. **Do not modify the code generated automatically by the IDE under `src/activity/`**
4. **The entry point must set the anti-brick flag** (see the "Notes" section of [`Z21_TC002_Demo/README.md`](Z21_TC002_Demo/README.md)):

   #include <os/SystemProperties.h>
   SystemProperties::setString("sys.zkapp.state", "running");

5. **`platform` in `Manifest.xml` must be `Z21`** (this is the TC002's platform identifier)

### Per-File License Declaration (Recommended)

Add an SPDX comment at the top of every `*.cpp` / `*.h` / `*.cc` file:

```cpp
// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (C) 2026 Your Name
```

---

## MQTT App Guidelines

### Directory Structure

```raw
apps/mqtt/<your-app-name>/
├── blueprint.yaml         # Required: the main HA blueprint file; metadata goes in the blueprint: section
├── icons/                 # Optional: 8x8 pixel icons for the TC002 display (*.png / *.gif)
├── preview/               # Required: at least one screenshot or GIF of it running
│   └── demo.gif
└── docs/                  # Required: README + license declaration + detailed documentation
    ├── README.md          # The app documentation (including the Open-in-HA button, parameter descriptions and the list of MQTT topics)
    └── LICENSE            # Optional: declare it here if the blueprint uses a different license from the repository's main one
```

> Unlike FlyThings apps, MQTT apps do **not** need a separate `manifest.json` — the metadata (name, description, author, license and so on) is carried by the `blueprint:` section of `blueprint.yaml` and the SPDX comment at the top of the file.
>
> **For non-HA-blueprint types** (Node-RED flows, openHAB rules and so on), replace `blueprint.yaml` with the corresponding core file, for example `flow.json` or `rule.yaml`; all other requirements are the same.

### Specific Requirements for Home Assistant Blueprints

#### 1. The blueprint file must be directly parseable by HA, with the metadata written into the file

```yaml
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2026 Your Name <your@email.com>
#
# TC002 Battery Monitor — flashes a reminder on the TC002 when the phone battery drops below a threshold
# Homepage: https://github.com/your-user/your-repo

blueprint:
  name: TC002 Battery Monitor
  description: |
    Flashes a reminder on the TC002 when the phone battery drops below a threshold.
    Author: Your Name <your@email.com>
    License: GPL-3.0-or-later
  domain: automation
  source_url: https://github.com/UlanziTechnology/Ulanzi-U-Clock-TC002/blob/main/apps/mqtt/battery-monitor/blueprint.yaml
  input:
    battery_sensor:
      name: Battery sensor
      selector:
        entity:
          domain: sensor
          device_class: battery
    # ...

trigger:
  - platform: numeric_state
    # ...

action:
  - service: mqtt.publish
    data:
      topic: tc002/notify
      payload: '{"text":"Low battery!","icon":42,"duration":10}'
```

- The top of the file **must** carry SPDX comment lines declaring the license and copyright
- `blueprint.name` / `blueprint.description` are required; it is recommended to repeat the author and license in the `description` section so that HA users see them directly in the import dialog
- `source_url` is **required** and should point to the blueprint's raw URL in this repository (the maintainers will help fill it in after merging)
- `domain` is usually `automation`, `script` or `template`
- All adjustable parameters go through the `input` section — **do not hard-code user values in trigger/action**

#### 2. `docs/README.md` must contain the one-click import button

Embed the "Open in HA" badge at the top of `docs/README.md`. Template:

```markdown
[![Open your Home Assistant instance and show the blueprint import dialog.](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FUlanziTechnology%2FUlanzi-U-Clock-TC002%2Fblob%2Fmain%2Fapps%2Fmqtt%2F<your-app-name>%2Fblueprint.yaml)
```

Replace `<your-app-name>` with your directory name, and escape every `/` in the URL as `%2F`.

After merging, clicking the badge takes the user to the import dialog in their own HA instance.

#### 3. MQTT Communication Conventions for the TC002

> ⚠️ **This section is still to be completed by Ulanzi:** the standard TC002 MQTT topics and payload schema will be published officially after firmware vX.Y.Z. Until the standard is published, please:
>
> - List all topics your blueprint publishes to / subscribes to, with example payloads, in `README.md`
> - Expose the broker address as a parameter in the blueprint's `input` — **do not hard-code it**
> - Always encode payloads as UTF-8 JSON

### Requirements Before Submitting

1. **It can be imported into HA successfully**: upload `blueprint.yaml` to your own HA instance, import it and run it successfully
2. **Real-device verification**: the screenshots or GIFs must show it running on a real TC002 device
3. **Parameterization**: every value a user might want to change (broker, topic, display text, thresholds, colors, etc.) goes through the blueprint's `input`
4. **No hard-coded credentials**: real broker passwords and API keys are **forbidden** in sample code, documentation and blueprint.yaml
5. **Icon naming**: if you submit `icons/`, use lowercase English + hyphens for the file names (`battery-low.png`) and list the icons in `docs/README.md` (MQTT apps have no `manifest.json`; the metadata is carried by `blueprint.yaml`)

---

## Submission Process

### 1. Fork & Clone

Click **Fork** on GitHub, then clone your fork:

```bash
git clone https://github.com/<your-username>/Ulanzi-U-Clock-TC002.git
cd Ulanzi-U-Clock-TC002
git remote add upstream https://github.com/UlanziTechnology/Ulanzi-U-Clock-TC002.git
```

### 2. Create a Branch

Branch naming convention: `<type>/<app-name>`, for example:

```bash
git checkout -b flythings/weather-clock
# or
git checkout -b mqtt/home-assistant-bridge
```

### 3. Develop and Commit

Build your app in the corresponding directory. The recommended commit message format:

```raw
<type>(<app-name>): <one-line summary>

An optional multi-line detailed description.

Signed-off-by: Your Name <your@email.com>
```

Examples:

```raw
feat(flythings/weather-clock): initial version with QWeather API support

- Shows the current temperature and weather icon on the home screen
- Three-day forecast on a secondary page
```
```raw
feat(mqtt/battery-monitor): add an HA blueprint for low phone battery alerts

- Watches any sensor with device_class=battery
- Pushes a flashing icon to the TC002 over MQTT when it drops below the threshold
```

### 4. Sync with Upstream and Push

```bash
git fetch upstream
git rebase upstream/main
git push origin flythings/weather-clock
```

### 5. Open a Pull Request

On GitHub, open a PR from your branch to `UlanziTechnology/Ulanzi-U-Clock-TC002:main`, with a title matching your first commit.

Please include the following in the PR description:

- [ ] App type: FlyThings / MQTT
- [ ] Verified on real hardware / the target environment
- [ ] Screenshots or a video of it running are attached
- [ ] The license is GPL-3.0 compatible
- [ ] I have read and followed this Contributing Guide

---

## Review Process

1. **Automated checks**: CI (where configured) validates the directory structure, the `manifest.json` fields, file sizes and so on
2. **Initial maintainer review**: a first round of feedback is usually given within 5 working days
3. **Iterating on changes**: just add commits to the same branch based on the feedback; do not open a new PR
4. **Merging**: after passing review, a maintainer squash merges it into `main`

If a PR gets no response for a long time, feel free to @ an Ulanzi team member in the PR or send a reminder in the issue tracker.

---

## Code of Conduct

- Be friendly, respectful and professional in issues, PRs and comments
- Personal attacks, harassment and discriminatory language are not accepted
- The maintainers reserve the right to act against offending accounts (warning / blocking / removing content)

---

## Contact

- **GitHub Issues**: technical questions, bug reports, feature suggestions
- **Official website**: <https://www.ulanzi.com>
- **Developer email**: (to be added)

We look forward to your great work 🎉
