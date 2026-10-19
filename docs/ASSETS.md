English | [简体中文](i18n/zh-CN/ASSETS.md)

# Assets and Attribution

## Upstream platform

This application uses board-support code and reference sources from [FoloToy/ai-passport](https://github.com/FoloToy/ai-passport). The original MIT license and copyright notice remain in the root [LICENSE](../LICENSE).

Dependency sources and exact resolved versions are recorded in `dependencies.lock`. ESP-IDF resolves those components separately; their respective upstream licenses apply.

## Artwork

`assets/images/moon-bowl-source.png` was generated with OpenAI image generation on October 17, 2026, from my moon-and-golden-bowl concept. It uses the circular, four-quadrant full-moon bowl emblem from my original design; it does not imply a brand affiliation.

`moon-bowl-240.png` and `moon_background.c` are generated 240×320 RGB565 derivatives used on the device. The embedded image occupies 153,600 bytes. No third-party photograph is used in this artwork.

## Device illustration

`assets/images/pocket-singing-bowl-device-preview.png` is a finished AI-generated README illustration combining the appearance of FoloToy AI Passport with the Pocket Singing Bowl interface. It is based on the [simulator body reference](https://apsimulator.folotoy.cn/assets/ai-passport-shell-reference.png) and a native rendering of the current B/crown screen. The full-device presentation was informed by the [PassportSim example](https://github.com/BlackHole1/passportsim). It is an illustration, not a photograph or a captured hardware session. Created with OpenAI image generation on October 19, 2026. The two bowl emblems were subsequently corrected to match the geometric four-quadrant mark in the original artwork, replacing an unintended floral motif.

## Sound

`assets/music/bowl-{C,D,E,F,G,A,B}.wav` and `moon-bowls.pcm` are original, reproducible modal-synthesis sounds created for this application and provided under the repository's MIT license. No external sound recording is used.

Each note is 16 kHz mono, signed 16-bit little-endian PCM, with eight seconds of audio. The combined bank contains 1,792,000 bytes. Regenerate it with `tools/generate_moon_audio.py`.

## Font

[Noto Sans CJK SC Regular](https://github.com/notofonts/noto-cjk/blob/main/Sans/OTF/SimplifiedChinese/NotoSansCJKsc-Regular.otf) is distributed under the SIL Open Font License 1.1, retained in [NotoSansCJK-LICENSE.txt](../assets/fonts/NotoSansCJK-LICENSE.txt).

The generated `moon_font_{12,14,18,28}.c` files are application-specific glyph subsets. The original font remains available for reproducible generation and is not embedded in full in the firmware. The font's own license remains applicable to its generated derivatives.
