# ViPERFX_RE

This is a reverse-engineering project aimed to make ViPER4Android more easy to access, modify, and improve.
Some QOL changes have been made, such as processing audio as float32, removing unused code, using more modern libraries, etc.

The new ViPER4Android APP source code will be release soon, I need some time to clean my codebase, which has a lot debug parts to test the decompiled effects from the original `libv4a_fx.so`.

I am also working on bringing the ViPER DSP to MacOS and Windows, please stay tuned.

# Building

- Install the NDK, CMake and Make.
- Run `make libs` to build .so files.
- Run `make zip` to generate Magisk module.

# Credits

Zhuhang and ViPER520 for making ViPER4Android
Martmists, Iscle, llsl for reverse-engineering
