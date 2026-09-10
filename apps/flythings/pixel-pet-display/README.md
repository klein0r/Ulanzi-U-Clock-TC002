# Pixel Pet Display

## Introduction

It would be a shame not to keep a pet on a pixel display! This app, called Pixel Pet, is a native FlyThings application running on the Pixbar TC002. It turns the TC002 into an interactive desktop pixel pet window, currently supporting three pets — cat, dog and rabbit — with automatic idle loops as well as walking, happy, eating and sleeping animations.

The app does not depend on a computer-side WebUI: the animation frames are compiled into the firmware as built-in C++ data, and button events are triggered directly by the buttons and knob on top of the device.

## Preview

The GIF below is generated from the same set of 52 x 16 pet animation frames used by this app, to preview the main interactions:

![Pixel Pet Display](preview/pixel-pet-display.gif)

The GIF covers the following actions:

- Switching between cat, dog and rabbit with the knob
- Left button triggers walking
- Middle button triggers happy
- Right button short press triggers eating
- Right button long press triggers sleeping

## Features

- Supports three pets: cat, dog and rabbit
- Each pet has an automatic life loop
- The three buttons on top trigger interactive animations
- The knob switches the current pet
- The feeding animation first walks the pet up to the food bowl, then makes the food disappear

## Controls

| Input | Action |
|---|---|
| Knob clockwise | Switch to the next pet |
| Knob counter-clockwise | Switch to the previous pet |
| Knob press | Return the current pet to its automatic loop |
| Left button | The current pet walks |
| Middle button | The current pet is happy |
| Right button short press | The current pet eats |
| Right button long press (approx. 800 ms or more) | The current pet sleeps |

## Dependencies

- Hardware: Ulanzi TC002
- Platform: Z21
- Development tool: FlyThings IDE
- Dependency packages: `easyui`, `log`, `zkhardware`, `base-utility`, `transfer-protocols`

The prototype version has been verified in the following environment:

- Device SN: `B0D191008U3670007`
- MCU version: `T1.0.13`
- App version: `0.2.9`

## Installation and Running

1. Install the FlyThings IDE.
2. Import the `apps/flythings/pixel-pet-display/` project into the IDE.
3. Update the dependency packages.
4. Build the project in the IDE.
5. Download and debug it on the TC002 over Wi-Fi ADB.
6. To flash it permanently, build the image to produce `update.img` and upgrade via a TF card as described in the official documentation.

## Configuration

The current version needs no external configuration.

## Implementation Notes

This app uses `PageBase::sendLedData()` to refresh the TC002's 52 x 16 RGB LED matrix directly. The pet animations come from pre-rendered 52 x 16 GIFs, converted by a script into lists of non-black pixels and compiled into `src/assets/PetAnimationFrames.h`.

To keep the runtime logic simple, the current version implements cat, dog and rabbit as three sibling pages within the same activity:

- `CatPetPage`
- `DogPetPage`
- `RabbitPetPage`

Turning the knob only switches the current pet, and the buttons on top only control the current pet's actions.

## Asset Sources and License

The base pet pixel assets come from CC0 resources on OpenGameArt, and have been re-cropped, recolored and combined into the TC002's 52 x 16 animation frames. For details see [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
Commercial use is permitted.
The code of this app is released under GPL-3.0-or-later.

## Known Issues

- The three sets of pet frame assets are packed as built-in C++ header files, which makes the source code fairly large.
- The current version does not support adding custom pets on the device.
- The current version does not persist the last selected pet; after a restart it goes back to the cat by default.
