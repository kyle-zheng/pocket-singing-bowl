English | [简体中文](docs/i18n/zh-CN/AGENTS.md)

# Contributor Instructions

This repository publishes Pocket Singing Bowl v0.1.8 for FoloToy AI Passport.

- Write personal development and listening experiences in the first person; do not refer to the project owner as "the creator" or "the author". Keep the seven-note/chakra mapping consistent with the implemented themes.
- Use English at default documentation paths and for repository metadata, commit messages, and release titles. Maintain Simplified Chinese translations under `docs/i18n/zh-CN/`, with reciprocal language links. Keep both versions factually aligned. Existing Simplified Chinese device labels are part of the current firmware interface.
- Use the complete hardware name **FoloToy AI Passport** in reader-facing prose. Preserve existing technical identifiers and build filenames.
- Keep hardware interfaces in `components/bsp/` and application behavior in `main/moon_*`.
- Preserve LVGL locking, non-blocking button callbacks, and coordinated worker shutdown.
- Run `python3 tools/run_host_tests.py` for relevant logic changes. Use ESP-IDF 5.5.3 for firmware builds and verify the merged image with `tools/verify_firmware.py`.
- Report build, host checks, and device testing separately; do not describe compilation as device validation.
- Do not commit credentials, personal device identifiers, local logs, build caches, or generated firmware images. Put downloadable firmware in Releases.
- Before every GitHub push, upload, or release, review the outgoing files and metadata for sensitive personal information. Include commit identities, archive contents, image metadata, and readable firmware strings when relevant; keep credentials outside the repository and use a GitHub noreply commit address. Verify the public result after publishing.
- Preserve the upstream MIT notice and the bundled font license.
- Do not flash a device, publish to an external account, or overwrite remote state without the user's authorization.
