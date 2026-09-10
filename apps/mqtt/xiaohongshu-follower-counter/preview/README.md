# Preview and Real-Device Evidence

`demo.png` is a 52×16 **render preview** produced by the same browser renderer as the Chrome extension. It is used to check the fixed logo, the digits, the K/M character matrix, the layout and the PNG encoding. It is not a photo of real hardware and cannot replace the real TC002 evidence required upstream.

Before the final merge, one of the following files should be added:

- `tc002-real.jpg`: clearly showing the TC002 hardware and the follower count on screen;
- `tc002-real.gif`: showing the real device before and after a Home Assistant MQTT update.

It is recommended to also record the HA automation trace and the broker topic, but the material must not expose the webhook ID, the MQTT password, the personal profile page, browser login pages or any other private information. Once real-device material has been added, reference it in the app README.
