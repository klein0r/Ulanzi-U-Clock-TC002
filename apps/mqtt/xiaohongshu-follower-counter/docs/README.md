# TC002 Xiaohongshu Follower Count: Installation and Operation

## How It Works

```text
The current Chrome login session
  → JSON embedded in the Xiaohongshu page / the visible "followers" DOM
  → A fixed pixel matrix + a 52×16 PNG generated in the browser
  → A local-only Home Assistant webhook
  → The Home Assistant MQTT integration
  → <devicePrefix>/custom/<appName>
  → The TC002 MQTT/DIY page
```

Based on the **TC002 device IP** entered by the user, the extension requests `/getBase` and `/getMqttConfig` and validates the IP, MAC, MQTT switch, broker address, port and prefix returned by the device. The `devicePrefix` is the device's `mqtt_prefix` plus an underscore and the last four digits of the MAC. It does not store MQTT credentials, and the username and password in the device's response are never sent to Home Assistant.

One blueprint instance supports multiple devices, but only publishes to the `allowed_device_prefixes` allowlist. The blueprint constructs the topic itself; the extension cannot specify an arbitrary topic or `app_name`.

![52×16 render preview](../preview/demo.png)

## Prerequisites

1. Home Assistant is installed with the **Home Assistant MQTT integration** enabled and connected to the same broker the TC002 uses.
2. MQTT/DIY is enabled on the TC002 in Ulanzi Studio; note that in some versions the DIY switch also affects the MQTT function.
3. Chrome can log in and open the target Xiaohongshu profile normally, and the computer can reach both the TC002 and Home Assistant.
4. This project has no Chrome Web Store version; the unpacked ZIP directory has to be loaded in developer mode.

## Import the Home Assistant Blueprint

[![Import the blueprint](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2Fklein0r%2FUlanzi-U-Clock-TC002%2Fblob%2Fdocs%2Ftranslate-to-english%2Fapps%2Fmqtt%2Fxiaohongshu-follower-counter%2Fblueprint.yaml)

If the My button does not work, paste the manual blueprint import URL under "Settings → Automations & Scenes → Blueprints → Import blueprint" in Home Assistant:

```text
https://github.com/klein0r/Ulanzi-U-Clock-TC002/blob/docs/translate-to-english/apps/mqtt/xiaohongshu-follower-counter/blueprint.yaml
```

Create the automation and configure:

- `webhook_id` / **Webhook ID**: a URL-safe random value of 24–128 characters. On macOS / Linux you can run `openssl rand -hex 24`; in Windows PowerShell, `[guid]::NewGuid().ToString("N")`. It acts as a shared secret — do not screenshot it, commit it or send it to anyone.
- `allowed_device_prefixes`: one per line. The following is only a format example, not a fixed parameter:

  ```text
  ulanzi_1be3
  ulanzi_1bd9
  ```

- `app_name`: the app name after `custom/`, default `xiaohongshu_followers`; only ASCII letters, digits, `_` and `-` are allowed.

The final topic formula is fixed as `<devicePrefix>/custom/<appName>`. For example, with the prefix and app name from the examples above, you get `ulanzi_1be3/custom/xiaohongshu_followers`.

## Download and Verify the Chrome Extension

Download:

- [xiaohongshu-follower-counter-chrome-0.2.0.zip](../release/xiaohongshu-follower-counter-chrome-0.2.0.zip)
- [SHA256SUMS](../release/SHA256SUMS)

Current SHA-256:

```text
5f499582bae5a766fb0c06d3add63650a54b98eb4f602919751eca532774c769
```

macOS:

```bash
shasum -a 256 ../release/xiaohongshu-follower-counter-chrome-0.2.0.zip
```

Linux:

```bash
sha256sum ../release/xiaohongshu-follower-counter-chrome-0.2.0.zip
```

Windows PowerShell:

```powershell
(Get-FileHash ..\release\xiaohongshu-follower-counter-chrome-0.2.0.zip -Algorithm SHA256).Hash.ToLower()
```

The output must match `SHA256SUMS`; then unzip the ZIP.

## Installing, Upgrading and Configuring in Chrome

1. Open `chrome://extensions` and enable "Developer mode".
2. Click "Load unpacked" and select the unpacked ZIP directory.
3. Open the extension's "Details → Extension options".
4. Enter the Home Assistant URL and the same webhook ID used by the blueprint.
5. Add one row per device with the TC002 device IP and the Xiaohongshu user profile URL.
6. The refresh interval has a minimum and a default of **5 minutes (300 seconds)**. On saving, Chrome only requests dynamic permissions for the HA host and the TC002 hosts you entered.

When upgrading, keep the same unpacked directory, replace the old files with the contents of the new ZIP, and then click "Reload" on the extension's card in `chrome://extensions`. The device/profile bindings and the refresh value are preserved. The local forwarding address and token from version 0.1.0 are removed; shorter refresh values are raised to 300 seconds automatically.

Each device may appear only once; several devices may be bound to the same profile. Profile URLs have their query and hash removed when saved, to avoid persisting a temporary `xsec_token`.

### Discovering the Device Prefix for the First Time

If the blueprint does not allow the device yet, HA will refuse to publish, but the "Latest result per device" section of the extension's options page still shows the dynamically discovered `devicePrefix`. Copy it into a new line of `allowed_device_prefixes`, save the HA automation and wait for the next refresh. You can also verify it manually from the `mqtt_prefix` returned by the TC002 and the last four digits of the MAC.

## Webhook and MQTT Data

The extension only sends the following to HA:

```json
{
  "devicePrefix": "ulanzi_1be3",
  "profileUrl": "https://www.xiaohongshu.com/user/profile/<profile-id>",
  "displayName": "Example user",
  "followerCount": 12800,
  "observedAt": "2026-07-21T12:00:00.000Z",
  "payload": "{\"duration\":31536000,\"text\":[],\"image\":[...],\"draw\":[]}"
}
```

After validating the prefix, the payload length and the app name, the blueprint calls `mqtt.publish` with QoS 0 and `retain: true`. The rendered content is a fixed color matrix for the logo, the digits, `.`, `K` and `M`, not a system font converted to pixels.

## Verifying the Evidence Chain

1. The extension's options page: the result for the device should be `ok: true` and show the `devicePrefix`, the nickname, the follower count and the time.
2. Home Assistant: open the **automation trace** for that automation and confirm that the webhook fired, the allowlist condition passed and `mqtt.publish` ran.
3. The MQTT broker: subscribe to the exact topic (e.g. `ulanzi_1be3/custom/xiaohongshu_followers`) and look at the retained message on that topic; the payload should contain `data:image/png;base64,iVBOR`.
4. The TC002: keep the MQTT/DIY function enabled, switch to that custom app, and check the actual screen.

An `HTTP 2xx` only means HA accepted the webhook; it does not prove that MQTT published or that the TC002 displayed anything. You must judge from the automation trace, the broker's retained message and the real device's screen together.

## Privacy and Security Boundaries

- It visits the page normally using the current Chrome session, but it does not read cookies, call the cookie API or export the login state.
- It does not bypass logins, captchas, anti-abuse systems, rate limits or access control.
- It does not store MQTT credentials and does not put the username/password from `/getMqttConfig` into the webhook.
- It does not need a Home Assistant long-lived access token; only the local-only webhook ID is stored.
- The webhook ID, device IPs, profile bindings and refresh value are only stored in `chrome.storage.local` on the current computer.
- Neither the source code nor the ZIP contains a hard-coded computer IP, a hard-coded profile, an MQTT password or an extension ID.

## Troubleshooting

### The follower count cannot be found

Open the profile manually in the same Chrome, complete the normal login or security check, and confirm that the "followers" number is visible on the page. When the page structure changes, the parser may need updating.

### `device_unreachable`

Check that the TC002 device IP is correct, that the computer and the device are on a mutually reachable local network, and that the browser can open `http://<device IP>/getBase` and `/getMqttConfig`. The corresponding host permission should have been granted when saving the configuration.

### `invalid_device_response` / `invalid_mqtt_config` / `mqtt_disabled`

Check that the TC002 returns its own IP and a valid MAC, that MQTT is enabled, that the broker is a valid LAN IPv4 address, and that the port and prefix are correct. Once corrected, the next request will rediscover it.

### `webhook_rejected`

Check the webhook ID and whether the HA automation is enabled. If the automation fired but the condition did not pass, copy the `devicePrefix` from the extension's result into `allowed_device_prefixes` and check that `app_name` is valid.

### `ha_unreachable`

Check the HA URL, LAN connectivity, the HTTPS certificate and Chrome's host permissions. The HA URL must not include the `/api/webhook/...` path.

### HA fired but the device shows nothing

Check `mqtt.publish` in the automation trace; look at the retained topic on the broker; make sure the TC002 uses the same broker, that the MQTT/DIY switch is on, and that the same custom app name is open.

## Development Checks

Node.js 20+ is required. Run the following in the app directory on Windows, macOS or Linux:

```bash
npm run check
npm run package:extension
```

`npm run check` recursively checks the JavaScript syntax and validates the version, the required files, the deterministic ZIP, the SHA-256 and the release safety constraints.

## Known Limitations

- This is not an official Xiaohongshu API; page changes may break the parser.
- When the page shows abbreviations such as `K` or `万`, you only get the approximate number that is publicly shown on the page.
- Chrome must be running with a valid login session.
- A formal upstream merge should still include a photo or GIF of the app running on real TC002 hardware.

License: GPL-3.0-or-later.
