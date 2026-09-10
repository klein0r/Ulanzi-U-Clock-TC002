# Quick Install

## 1. Prepare Home Assistant

First configure the **Home Assistant MQTT integration** so that it connects to the same MQTT broker the TC002 is using.

[![Import the Home Assistant blueprint](https://my.home-assistant.io/badges/blueprint_import.svg)](https://my.home-assistant.io/redirect/blueprint_import/?blueprint_url=https%3A%2F%2Fgithub.com%2FUlanziTechnology%2FUlanzi-U-Clock-TC002%2Fblob%2Fmain%2Fapps%2Fmqtt%2Fxiaohongshu-follower-counter%2Fblueprint.yaml)

When creating the automation, fill in:

- a random, secret `Webhook ID`;
- `allowed_device_prefixes`, one TC002 prefix per line;
- `app_name`, default `xiaohongshu_followers`.

## 2. Install the Chrome Extension

Download [xiaohongshu-follower-counter-chrome-0.2.0.zip](../release/xiaohongshu-follower-counter-chrome-0.2.0.zip) and [SHA256SUMS](../release/SHA256SUMS), verify the checksum and unzip.

Open `chrome://extensions`, enable developer mode, click "Load unpacked" and select the unpacked directory. This version has not yet been published to the Chrome Web Store.

## 3. Configure the Devices

On the extension's options page, enter the Home Assistant address, the same webhook ID, and for each TC002 its device IP and the corresponding Xiaohongshu profile. Save, and allow Chrome to access the LAN address you entered.

If the allowlist does not match on the first attempt, "Latest result" will still show the discovered `devicePrefix`; copy it into `allowed_device_prefixes` and save the HA automation. The refresh interval has a minimum of 5 minutes (300 seconds), to reduce the risk of Xiaohongshu rate limiting or blocking.

Make sure the TC002's MQTT/DIY function stays enabled. For the full evidence chain and troubleshooting, see the [detailed documentation](README.md).
