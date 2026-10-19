# Architecture

## Application and board support

`main/CMakeLists.txt` links the `moon_*` application, generated fonts, image data, and the PCM bank. The original demo sources remain in `main/` as upstream reference and are not part of the application build. Hardware support lives in `components/bsp/`.

`moon_model.c` is a platform-independent state machine. Input callbacks enqueue events, and the controller applies them without doing audio or storage work inside a button callback. UI changes take the LVGL lock. Separate workers handle audio streaming and preferences/battery work.

## Sound

Seven original modal-synthesis samples are stored as 16 kHz, mono, 16-bit PCM, eight seconds per note. Four voices may overlap. A smooth onset, per-note headroom, tail crossfades, and a lookahead limiter reduce abrupt transitions and overload during repeated playback.

The 0–100% volume scale uses the v0.1.7 tuned gain curve: the lower range retains its original gain, and the upper range reaches a reduced ceiling. Per-note limits also bound mixed output. The 35% curve knee is independent of the v0.1.8 **50% default**. A 100% setting means the tuned application ceiling, not a measured acoustic maximum for every device.

## Preferences and power

Preferences are stored in NVS after a short debounce. Startup reads saved preferences and enters silent manual standby. The initial defaults are B/crown, 50% volume, six-second intervals, three-minute automatic sessions, and automatic sleep off.

When enabled, automatic sleep follows ten idle minutes and a brief backlight fade. Active automatic playback prevents idle sleep. Shutdown coordinates the workers and peripherals before deep sleep. The battery gauge remains active so a wake does not unnecessarily reset its state-of-charge estimation. Initial and abrupt battery readings are confirmed before being shown.

A function-key wake gesture is consumed, including delayed clicks, until the keys have been released long enough. The app returns to manual standby. The physical power key is independent of the application.

## Repository layout

```text
main/             Application sources and unlinked upstream demo references
components/bsp/   Display, audio, button, battery, and shared-bus support
assets/           Artwork, generated fonts, PCM bank, and source WAV files
tests/            Host checks, stubs, and optional native UI renderer
tools/            Asset generation, host checks, and firmware verification
docs/             English user, build, architecture, asset, and release notes
```

The publication package preserves the build inputs from the verified v0.1.8 development tree. Public-facing documentation and repository automation are organized for this standalone application; development caches, device logs, credentials, and unrelated project archives are excluded.
