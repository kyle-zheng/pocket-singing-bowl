# Assets and Attribution

## Upstream platform

This application uses board-support code and reference sources from [FoloToy/ai-passport](https://github.com/FoloToy/ai-passport). The original MIT license and copyright notice remain in the root [LICENSE](../LICENSE).

Dependency sources and exact resolved versions are recorded in `dependencies.lock`. ESP-IDF resolves those components separately; their respective upstream licenses apply.

## Artwork

`assets/images/moon-bowl-source.png` was generated with OpenAI image generation on October 17, 2026, from the creator's approved moon-and-golden-bowl concept. The circular bowl mark is illustrative and has not been verified as an official logo. No brand endorsement is claimed.

`moon-bowl-240.png` and `moon_background.c` are generated 240×320 RGB565 derivatives used on the device. The embedded image occupies 153,600 bytes. No third-party photograph is used in this artwork.

## Sound

`assets/music/bowl-{C,D,E,F,G,A,B}.wav` and `moon-bowls.pcm` are original, reproducible modal-synthesis sounds created for this application and provided under the repository's MIT license. No external sound recording is used.

Each note is 16 kHz mono, signed 16-bit little-endian PCM, with eight seconds of audio. The combined bank contains 1,792,000 bytes. Regenerate it with `tools/generate_moon_audio.py`.

## Font

[Noto Sans CJK SC Regular](https://github.com/notofonts/noto-cjk/blob/main/Sans/OTF/SimplifiedChinese/NotoSansCJKsc-Regular.otf) is distributed under the SIL Open Font License 1.1, retained in [NotoSansCJK-LICENSE.txt](../assets/fonts/NotoSansCJK-LICENSE.txt).

The generated `moon_font_{12,14,18,28}.c` files are application-specific glyph subsets. The original font remains available for reproducible generation and is not embedded in full in the firmware. The font's own license remains applicable to its generated derivatives.
