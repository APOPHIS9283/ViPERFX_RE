# ViPERFX_RE

A reverse-engineered, modernized port of ViPER4Android. The DSP has been re-implemented from a decompilation of the original `libv4a_fx.so`, with audio processed in float32, dead code removed, and dependencies refreshed.

If you want ViPER on desktop instead of Android, see:

- [ViPER4Windows](https://github.com/likelikeslike/ViPER4Windows)
- [ViPER4Mac](https://github.com/likelikeslike/ViPER4Mac)

## Module Architecture

ViPERFX_RE ships as **two separate Magisk modules**, both built around the same DSP engine (`ViPERDSP`) but integrated through different Android audio HAL interfaces. They are not interchangeable: each module is designed for a specific generation of Android’s audio framework. Installing the wrong one may either do nothing or cause boot issues.

### Non-AIDL (Legacy) Module

The non-AIDL module uses the **classic Android audio effect plugin interface** (often informally referred to as the “HIDL-based” path due to its association with the older audio HAL stack).

- **Why it no longer works on Android 15+:** Google has migrated the audio effect framework from the legacy C-based plugin model to a stable AIDL HAL implementation. Devices launching with Android 15 no longer support loading `effect_handle_t` plugins, as the old framework path has been removed entirely. See [AIDL for HALs](https://source.android.com/docs/core/architecture/aidl/aidl-hals) for more details.

### AIDL Module

The AIDL module implements the **modern audio effect HAL** introduced in Android 13, which became the only officially supported audio effect path for devices launching with Android 15 and later.

### Which module should you install?

Run the following command:

```bash
adb shell ps -A | grep "audio.*aidl"
```

If your see any process related to audio HAL with "aidl" in the name, you need the AIDL module. If not, the non-AIDL module should work.

## Disclaimers

- **Tested hardware is narrow.** Non-AIDL is confirmed on Pixel 8 Pro / Android 14. AIDL is confirmed on Pixel 8 Pro / Android 16. Other devices may bootloop, may need vendor-specific shims (e.g. ShadoV's [PIXAML](https://github.com/ShadoV90/PIXAML) for AIDL on some Pixels), or may simply do nothing. Make a backup before flashing.
- **Audio fidelity is best-effort.** The DSP is decompiled from `libv4a_fx.so`. Subtle deviations from the original ViPER4Android are expected. If you spot one, please open a PR — the source is here precisely so it can be improved.
- **Not for commercial use.** This is a reverse-engineering project and may carry legal restrictions in your jurisdiction. Use at your own risk.

## Installation

1. Download the **module zip matching your Android version** (see the table above) from the [Releases page](https://github.com/likelikeslike/ViPERFX_RE/releases), and the [ViPER4Android app](https://github.com/likelikeslike/ViPER4Android).
2. Flash the Magisk module. **Do not flash both modules.**
3. Install the app.
4. Reboot. Open the app and verify effects are applied (use any of the diagnostic commands below to confirm).

## AIDL Driver Troubleshooting

> [!NOTE]
> The AIDL module needs to mount the driver `.so` and `audio_effects*.xml` into `/vendor` and `/system`. This is verified with **MagiskSU**. If you use **KernelSU** or **APatch**, you may need to install any metamodule that allows mounting files into `/vendor` and `/system`.

> [!IMPORTANT]
> When opening an issue, capture the relevant logs *while reproducing the problem* and attach them. The commands below are listed in the order you should run them when troubleshooting.

### Log Tags

| Driver | Tag                  | Level   |
| ------ | -------------------- | ------- |
| AIDL   | `ViPER4Android_AIDL` | I/W/E   |
| AIDL   | `ViPER4Android`      | D/I/E   |
| AIDL   | `AHAL_EffectImpl`    | D/I/V/E |
| AIDL   | `AHAL_EffectContext` | E       |
| AIDL   | `AHAL_EffectThread`  | V       |

### 1. Check if the library is loaded

```bash
adb logcat -s 'ViPER4Android_AIDL:*' 'ViPER4Android:*' | grep -E 'Welcome|viperLibrary|created'
```

Expected:

```bash
ViPER4Android_AIDL: ViperAidlEffect created
ViPER4Android: Welcome to ViPER FX
ViPER4Android: Current version is 1.x.x (202xxxxx)
```

If missing, the audio framework never loaded the `.so`. Verify the config XML was patched:

```bash
adb shell cat /vendor/etc/audio_effects_config.xml | grep v4a
```

Expected:

```bash
<library name="v4a_aidl" path="libv4a_aidl.so"/>
<effect name="v4a_standard_aidl" library="v4a_aidl" uuid="90380da3-..." type="7261676f-..."/>
```

And verify the SELinux label on the library itself:

```bash
adb shell su -c 'ls -Z /vendor/lib64/soundfx/libv4a_*.so'
```

Expected:

```bash
u:object_r:vendor_file:s0 /vendor/lib64/soundfx/libv4a_aidl.so
```

### 2. Check if SHM files exist and have correct permissions

```bash
adb shell ls -laZ /data/local/tmp/v4a/
```

Expected:

```bash
-rw-rw-rw- 1 audioserver audio u:object_r:shell_data_file:s0   32784 ... shm_hp.bin
-rw-rw-rw- 1 audioserver audio u:object_r:shell_data_file:s0   32784 ... shm_spk.bin
-rw-rw-rw- 1 audioserver audio u:object_r:shell_data_file:s0     256 ... shm_status.bin
```

Inspect SHM header (magic number check):

```bash
adb shell xxd -l 16 /data/local/tmp/v4a/shm_status.bin
```

Expected:

```bash
00000000: 534d 3456 0300 0000 0100 0000 0100 0000  SM4V............
```

#### 3. Check SELinux denials

```bash
adb logcat -d -s audit | grep v4a
# or, broader:
adb logcat -d | grep -E 'avc.*denied.*(v4a|shm|shell_data_file)'
```

If you see `avc: denied` lines naming the HAL process and the SHM files, the live policy injection from `post-fs-data.sh` did not stick. This is the single most common cause of "module installs cleanly but audio is unprocessed."

#### 4. Dump audioserver effect list

```bash
adb shell dumpsys media.audio_flinger | grep -E -B2 -A10 'Effect ID|ViPER|v4a|90380da3'
```

Expected (effect loaded on session xxx):

```bash
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

Expected (Pixel 8 Pro):

```bash
audioserver   1044     1  ...  S android.hardware.audio.service-aidl.aoc
audioserver   1107     1  ...  S audioserver
```

Then filter logcat by the HAL PID:

```bash
adb logcat --pid=$(adb shell pidof android.hardware.audio.service-aidl.aoc | tr -d '\r') -s 'ViPER4Android_AIDL:*' 'ViPER4Android:*'
```

**Note that the process name is device-specific.**

## Building

Prerequisites: Android NDK, CMake, Make. Set `ANDROID_NDK_HOME` (or `ANDROID_NDK_ROOT`).

```bash
make libs   # build libv4a_re.so for arm64-v8a and armeabi-v7a
make zip    # build + package a flashable Magisk module zip
```

## Credits

- Zhuhang and ViPER520 — original ViPER4Android.
- Martmists, Iscle, llsl — reverse-engineering of the DSP.
