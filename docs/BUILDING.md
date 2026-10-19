# Building and Testing

## Requirements

- FoloToy AI Passport (ESP32-C3, 8 MB flash).
- ESP-IDF **5.5.3**, with its ESP32-C3 tools installed and its environment activated.
- Git, Python 3.10 or newer, and a C compiler for host checks.

Use Espressif's [ESP-IDF getting-started guide](https://docs.espressif.com/projects/esp-idf/en/v5.5.3/esp32c3/get-started/index.html) to prepare the toolchain. Runtime dependencies are pinned in `dependencies.lock`. The first dependency resolution needs Internet access; the installed application itself works offline.

## Build

From this repository's root, in an activated ESP-IDF shell:

```sh
idf.py build
idf.py merge-bin -o build/FoloToy-AI-Passport-full.bin
python3 tools/verify_firmware.py build
```

The target is already set to ESP32-C3 in `sdkconfig.defaults`. The CMake project retains the upstream artifact prefix, so the merged output is named `FoloToy-AI-Passport-full.bin`.

## Install

The easiest installation route is the [community project](https://ai-passport.folotoy.cn/plays/1384/). A browser simulator is also [available](https://apsimulator.folotoy.cn/?play=1384).

For a local development build:

```sh
idf.py -p YOUR_SERIAL_PORT flash
```

This flashes the build's individual images. For a published, complete merged image at offset zero, use the instructions supplied with that release. A complete merged refresh can replace stored settings with defaults. Save anything you need before replacing another application.

## Host checks

```sh
python3 tools/run_host_tests.py
```

The checks cover the application state machine, interval and volume editing, mute, automatic sessions, mixer boundaries, wake guarding, battery filtering, generated assets, and reusable BSP logic. They require no connected device and use only Python's standard library and a C compiler.

## Regenerate assets (optional)

Generated assets are already committed and do not need to be regenerated to build firmware. To regenerate them, install Pillow, numpy, and fonttools in a local virtual environment:

```sh
python3 -m venv .venv
. .venv/bin/activate
python3 -m pip install Pillow numpy fonttools
python3 tools/generate_moon_assets.py
```

To regenerate audio alone:

```sh
python3 tools/generate_moon_audio.py
```

Asset generation intentionally changes generated files; review those changes before committing.

## Validation boundaries

Host checks and a successful build do not establish speaker quality, battery accuracy, or sleep current. After changes to audio or power, test every note, repeated presses, automatic playback, mute, saved settings, and repeated sleep/wake cycles on the actual device.
