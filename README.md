# ViPERFX_RE

This is a reverse-engineering project aimed to make ViPER4Android more easy to access, modify, and improve.
Some QOL changes have been made, such as processing audio as float32, removing unused code, using more modern libraries, etc.

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

## AIDL Driver Troubleshooting

> [!NOTE]
> The AIDL module needs to mount the driver files and `audio_effects*.xml` files, and it is only tested with `MagiskSu`. If you are using other root solutions, such as `KernelSU`, make sure you have enable the corresponding options to allow mounting files in `/vendor` and `/system`.

> [!IMPORTANT]
> If you encounter issue with the installation or usage of the AIDL driver, you can diagnose the problem with the following commands. Make sure to capture the logs while reproducing the issue, and provide the logs if you want to ask for help in the issue tracker.

### Log Tags

| Driver | Tag                  | Level   |
| ------ | -------------------- | ------- |
| AIDL   | `ViPER4Android_AIDL` | I/W/E   |
| AIDL   | `ViPER4Android`      | D/I/E   |
| AIDL   | `AHAL_EffectImpl`    | D/I/V/E |
| AIDL   | `AHAL_EffectContext` | E       |
| AIDL   | `AHAL_EffectThread`  | V       |

### Basic Diagnostic Commands

#### 1. Check if the library is loaded

```bash
adb logcat -s 'ViPER4Android_AIDL:*' 'ViPER4Android:*' | grep -E 'Welcome|viperLibrary|created'
```

**Expected**:

```
ViPER4Android_AIDL: ViperAidlEffect created
ViPER4Android: Welcome to ViPER FX
ViPER4Android: Current version is 1.x.x (202xxxxx)
```

**If missing:** The library is not being loaded by the audio framework.

Verify the config XML is patched:

```bash
adb shell cat /vendor/etc/audio_effects_config.xml | grep v4a
```

**Expected:**

```log
<library name="v4a_aidl" path="libv4a_aidl.so"/>
<effect name="v4a_standard_aidl" library="v4a_aidl" uuid="90380da3-..." type="7261676f-..."/>
```

Verify SELinux context:

```bash
adb shell su -c 'ls -Z /vendor/lib64/soundfx/libv4a_*.so'
```

**Expected:**

```log
u:object_r:vendor_file:s0 /vendor/lib64/soundfx/libv4a_aidl.so
```

#### 2. Check if SHM files exist and have correct permissions

```bash
adb shell ls -laZ /data/local/tmp/v4a/
```

**Expected:**

```log
-rw-rw-rw- 1 audioserver audio u:object_r:shell_data_file:s0   32784 ... shm_hp.bin
-rw-rw-rw- 1 audioserver audio u:object_r:shell_data_file:s0   32784 ... shm_spk.bin
-rw-rw-rw- 1 audioserver audio u:object_r:shell_data_file:s0     256 ... shm_status.bin
```

Inspect SHM header (magic number check):

```bash
adb shell xxd -l 16 /data/local/tmp/v4a/shm_status.bin
```

**Expected:**

```log
00000000: 534d 3456 0300 0000 0100 0000 0100 0000  SM4V............
```

#### 3. Check SELinux denials

```bash
adb logcat -d -s audit | grep v4a
# or
adb logcat -d | grep -E 'avc.*denied.*(v4a|shm|shell_data_file)'
```

SELinux denials on the SHM files will prevent the HAL from reading params. The `post-fs-data.sh` script injects live policy rules, but they may not persist across all scenarios.

#### 4. Dump audioserver effect list

```bash
adb shell dumpsys media.audio_flinger | grep -E -B2 -A10 'Effect ID|ViPER|v4a|90380da3'
```

**Expected (effect loaded on session 0):**

```log
  Effect ID xxx:
    Session State Registered Internal Enabled Suspended:
    00000   002   y          n        y       n
    Descriptor:
    - UUID: 90380da3-8536-4744-a6a3-5731970e640f
    - TYPE: 7261676f-6d75-7369-6364-28e2fd3ac39e
    - name: ViPER4Android
    - implementor: ViPER520 / RE Team
```

#### 5. Filter by audio HAL process

```bash
# Find the audio HAL process name (device-specific)
adb shell ps -A | grep audio
```

**Expected (Pixel 8 Pro):**

```log
audioserver   1044     1  ...  S android.hardware.audio.service-aidl.aoc
audioserver   1107     1  ...  S audioserver
```

Then filter logcat by the HAL PID:

```bash
adb logcat --pid=$(adb shell pidof android.hardware.audio.service-aidl.aoc | tr -d '\r') -s 'ViPER4Android_AIDL:*' 'ViPER4Android:*'
```

**Note that the process name is device-specific.**

## Building

- Install the NDK, CMake and Make.
- Run `make libs` to build .so files.
- Run `make zip` to generate Magisk module.

## Credits

Zhuhang and ViPER520 for making ViPER4Android

Martmists, Iscle, llsl for reverse-engineering
