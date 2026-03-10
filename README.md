# ViPERFX_RE

This is a reverse-engineering project aimed to make ViPER4Android more easy to access, modify, and improve.
Some QOL changes have been made, such as processing audio as float32, removing unused code, using more modern libraries, etc.

The new ViPER4Android App source code will be release soon, I need some time to clean my codebase, which has a lot debug parts to test the decompiled effects from the original `libv4a_fx.so`.

I am also working on bringing the ViPER DSP to MacOS and Windows, please stay tuned.

## Important Notes

- The non-AIDL module is tested on Pixel 8 Pro running Android 14, it is not guaranteed to work on other devices or Android versions, and it may cause issues such as bootloops, crashes, or audio problems, so please make sure you have a backup and know how to uninstall the module if something goes wrong.
- The non-AIDL module does not work on some Android 15+ devices that use AIDL as the audio effect interface, see this [issue](https://github.com/AndroidAudioMods/ViPER4Android/issues/101#issuecomment-2358421687).
- The AIDL module is tested on Pixel 8 Pro running Android 16, it may not work on non-Pixel devices, some devices may need ShadoV's `PIXAML` module to work. And it is highly experimental.
- The effect algorithms are decompiled from the original `libv4a_fx.so` and may not be 100% accurate, and it is based on personal research and testing, so if you feel the sound is different from the original ViPER4Android, you may take a look at the source code in this repository and compare it with the original one, and contributions are welcome.
- This is reverse-engineering work, it cannot be used for any commercial purposes, and it may be illegal in some regions, so please make sure you understand the risks before using or modifying this project.

## Installation

- Download the latest module from the [releases page](https://github.com/likelikeslike/ViPERFX_RE/releases) and the ViPER4Android app from this [repo](https://github.com/likelikeslike/ViPER4Android)
- Install the Magisk module (make sure you install the correct version for your device) and the ViPER4Android app.
- Reboot your device and enjoy the new ViPER4Android.

## Building

- Install the NDK, CMake and Make.
- Run `make libs` to build .so files.
- Run `make zip` to generate Magisk module.

## Credits

Zhuhang and ViPER520 for making ViPER4Android

Martmists, Iscle, llsl for reverse-engineering
