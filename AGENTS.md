# Contributor Instructions

This repository publishes Electronic Singing Bowl v0.1.8 for FoloToy AI Passport.

- Use English for public documentation, repository metadata, commit messages, issues, and release notes. Existing Simplified Chinese device labels are part of the current firmware interface.
- Keep hardware interfaces in `components/bsp/` and application behavior in `main/moon_*`.
- Preserve LVGL locking, non-blocking button callbacks, and coordinated worker shutdown.
- Run `python3 tools/run_host_tests.py` for relevant logic changes. Use ESP-IDF 5.5.3 for firmware builds and verify the merged image with `tools/verify_firmware.py`.
- Report build, host checks, and device testing separately; do not describe compilation as device validation.
- Do not commit credentials, personal device identifiers, local logs, build caches, or generated firmware images. Put downloadable firmware in Releases.
- Preserve the upstream MIT notice and the bundled font license.
- Do not flash a device, publish to an external account, or overwrite remote state without the user's authorization.
