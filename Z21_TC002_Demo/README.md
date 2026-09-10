# Sample Code Documentation

This document describes how to use the various hardware features of this platform and what to watch out for. It is best read together with the source code of the corresponding example pages.

## Hardware Feature Overview

### Button Input

This chip provides one knob and three separate buttons, supporting the following input events in total (see `managers/KeyManager.h` for details):

| Key code | Description |
|---|---|
| `E_KEYCODE_CLOCKWISE` | Knob turned clockwise |
| `E_KEYCODE_ANTI_CLOCKWISE` | Knob turned counter-clockwise |
| `E_KEYCODE_KNOB_BUTTON` | Knob pressed |
| `E_KEYCODE_LEFT_BUTTON` | Left button pressed |
| `E_KEYCODE_MIDDLE_BUTTON` | Middle button pressed |
| `E_KEYCODE_RIGHT_BUTTON` | Right button pressed |

Register a callback with `KeyManager::getInstance().addKeyEventCallback(cb)` to receive button events. See `pages/BtnTestPage.h` for an example.

### LED Display

The LED panel has a resolution of **52×16** pixels and is driven over the SPI bus. Call `PageBase::sendLedData(rgbData)` with one frame of RGB data (arranged row by row, 3 bytes per pixel) to refresh the display. This interface already handles SPI initialization and the synchronization of `GPIO_35` internally, so you can call it directly. See `pages/RgbTestPage.h` for an example.

### Audio Playback

`AudioManager` (`managers/AudioManager.h`) controls audio playback, pausing, stopping and volume:

```cpp
#include "managers/AudioManager.h"
auto& audio = awtrix::AudioManager::getInstance();
audio.setVolume(3);          // Volume 0~6, 0 is muted
audio.playAudio("/path/to/file.mp3");
audio.pauseAudio();
audio.resumeAudio();
audio.stopAudio();
```

See `pages/AudioTestPage.h` for an example.

### Microphone Level Detection

The microphone level data is reported by the MCU and read through `McuManager` (`managers/McuManager.h`):

```cpp
#include "managers/McuManager.h"
McuManager::getInstance().setAutoMicReport(true);  // Enable automatic reporting
int micValue = McuManager::getInstance().queryMicValue();
```

`AudioTestPage` shows a complete implementation that displays the microphone level percentage in real time; see `pages/AudioTestPage.h`.

### WIFI + BLE

WiFi and BLE are both integrated, and there is a dependency between them: **BLE only works properly if WiFi has already been enabled**.

**Starting WiFi:**
```cpp
#include <base/wifi.h>
base::wifiOnAndWait(10);  // Wait for WiFi to be ready, 10 second timeout
```

**Starting BLE:**
```cpp
#include "ble/bluetooth_service.h"
BluetoothParams params;
params.name = "MyDevice";
params.on_message = [](const std::string& msg) {
    // Handle the received BLE message
};
BluetoothService::instance().start(params);
```

The WiFi and BLE parameters can be preset through the configuration file `/mnt/usb1/test.cfg` (JSON format), with the following fields:

```json
{
  "ssid":      "your_wifi_ssid",
  "pwd":       "your_wifi_password",
  "isConnect": true,
  "ble":       "MyDeviceName"
}
```

If the `ble` field is empty, the device name is generated automatically from the last four digits of the WiFi MAC address (e.g. `Ulanzi TC002 AB12`). See `pages/WifiTestPage.h` for an example.

### GPIO Interface

The two LED pins `GPIO_06` and `GPIO_85` are reserved for free use and are controlled through `utils/GpioHelper.h`:

```cpp
#include "utils/GpioHelper.h"
GpioHelper::output("GPIO_06", 1);  // High level turns the LED on
GpioHelper::output("GPIO_06", 0);  // Low level turns the LED off
```

## Notes

- Please read the relevant IDE documentation and development guide first to understand the build and firmware upgrade process.

- **Anti-brick check**: after the system starts, the running flag must be set at the entry point, otherwise the system will trigger the anti-brick rollback.

```cpp
#include <os/SystemProperties.h>
SystemProperties::setString("sys.zkapp.state", "running");
```

- **MCU initialization**: at startup the MCU communication must be initialized and the version number queried first, otherwise the LED panel will not work properly.

```cpp
#include "managers/McuManager.h"
McuManager::getInstance().initialize(new PixelMcuProto::McuParse("/dev/ttyS1", 1500000));
std::string mcuVer;
McuManager::getInstance().queryMcuVersion(mcuVer);
```

- **SPI frame rate limit**: the interval between frames must not be shorter than 15 ms, otherwise the LED display may misbehave. `sendLedData` already has built-in throttling, so calling it directly is recommended.
- **BLE depends on WiFi**: before using BLE features, make sure WiFi has been enabled successfully, otherwise the BLE service will fail to start.
- **Reflashing manually**: to flash the official firmware back, press and hold the reset button on the device (next to the USB-C port); the official firmware will be restored automatically.
