English | [简体中文](i18n/zh-CN/RELEASE_NOTES.md)

# Release Notes

## v0.1.8

- Start new installations with **B / Crown** and **50% volume**.
- Keep the improved v0.1.7 timbre, gain curve, and overlap control.
- Preserve existing saved preferences on an ordinary restart.

## Improvements included since the initial community release

- Softer attacks and rebalanced tones reduce harshness, especially in the lower notes.
- Improved overlapping-tone mixing and a tuned maximum output reduce crackling during repeated playback.
- The volume range remains 0–100%, including persistent mute, with accelerated adjustment when holding UP or DOWN.
- Battery-reading confirmation and sleep/wake handling improve recovery after idle sleep.

## Installation and validation

The packaged full image is the previously built and flashed v0.1.8 image. Its SHA-256 is:

```text
345e973c2044f77510625c36e9491ebc21063dfb02b9faea77e0feceb7958d11
```

The original development validation passed the firmware build, host checks, merged-image verification, and flash verification. Startup logs confirmed B/crown and 50% volume. The creator reported that the preceding v0.1.7 tuning eliminated the crackling in their listening test. Exhaustive acoustic testing and whole-device sleep-current measurements remain unperformed.

This GitHub publication organizes source files, bilingual documentation, and downloads; it does not introduce a new firmware version or modify the v0.1.8 runtime.
