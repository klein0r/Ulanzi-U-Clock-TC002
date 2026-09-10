# Third-Party Notices

The main code of this repository is released under the [GPL-3.0-or-later](LICENSE) license.
The repository also contains a number of third-party components, each governed by its original license. As required by the original authors, this file provides notices for those components.

If anything is missing or inaccurate, please feel free to open an issue or a PR.

---

## 1. BlueZ — The Linux Bluetooth Protocol Stack

**Location:**

- Binary tools: `Z21_TC002_Demo/src/dependencies/bin/`
  - `hciattach`, `hciattach_bk`, `hciattach_bk2`, `hciconfig`, `hcitool`, `gattserverbin`
- Header files: `Z21_TC002_Demo/src/dependencies/include/ble/`
- Static library: `Z21_TC002_Demo/src/dependencies/lib/libgatt-server.a`

**License:**

- The binary tools come from the command-line tools part of BlueZ and are governed by the **GNU General Public License v2.0 or later (GPL-2.0-or-later)**
- The header files and `libgatt-server.a` come from the library part of BlueZ and are governed by the **GNU Lesser General Public License v2.1 or later (LGPL-2.1-or-later)**

**Copyright:** Copyright © BlueZ project authors

**Source:** <https://git.kernel.org/pub/scm/bluetooth/bluez.git>

**Full license texts:**

- GPL-2.0: <https://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>
- LGPL-2.1: <https://www.gnu.org/licenses/old-licenses/lgpl-2.1.txt>

> As required by the GPL/LGPL, anyone who obtains a copy of this repository is entitled to obtain the complete source code of the above components under the terms of the corresponding licenses.
> This repository's main license, GPL-3.0, is compatible with the components listed above.

---

## 2. Adafruit GFX Font — TomThumb

**Location:** `Z21_TC002_Demo/src/Fonts/TomThumb.h`

**License:** BSD-style (the Adafruit and original author copyright notices are retained)

**Copyright:**

- Original 3x5 font: Copyright © Brian J. Swetland
- 8-bit conversion: Copyright © Robey Pointer
- Adafruit GFX adaptation: Copyright © Adafruit Industries

**Source:** <https://github.com/adafruit/Adafruit-GFX-Library>

---

## 3. FlyThings SDK and Its Dependency Packages

**Note:** This repository does **not distribute** the FlyThings SDK or its dependency packages directly; it only references them by package name and version number in `Z21_TC002_Demo/Manifest.xml`.
At build time the FlyThings IDE fetches them automatically from its package repository at <https://package.flythings.cn/>.

**Packages involved:** `easyui`, `log`, `zkhardware`, `zknet`, `base-utility`, `transfer-protocols`, `audio-utility`, `ffmpeg`, `z`, `base-json`

**Licenses:** the license of each package is as stated in the official FlyThings documentation. Among them, `ffmpeg` is usually **LGPL-2.1+** or **GPL-2.0+** (depending on the build options); for the remaining FlyThings-owned packages please refer to <https://www.flythings.cn>.

---

## 4. Test Resources

**Location:**

- `Z21_TC002_Demo/resources/1KHZ.MP3` — a 1 kHz test sine wave audio file, used in the `AudioTestPage` demo
- `IDE使用说明/resources/ADBDriverInstaller.zip` — a third-party ADB driver installer
- `IDE使用说明/resources/z11sproject-configuration.zip` — an example IDE project configuration

**Note:** These resources are used for teaching and demonstration purposes only; the copyright belongs to the original authors. If they infringe on your rights, please contact the repository maintainers to have them removed.

---

## 5. Documentation and Screenshots

**Location:** all `*.png`, `*.jpg` and `*.gif` files under `IDE使用说明/resources/`

**Note:** The screenshots show the interfaces of the FlyThings IDE and third-party software; the copyright belongs to the respective software authors, and they are used solely to illustrate the accompanying technical documentation.

---

## Acknowledgements

Thanks to the open source communities and vendors behind BlueZ, Adafruit, FlyThings, AWTRIX and others, whose work made this project possible.
