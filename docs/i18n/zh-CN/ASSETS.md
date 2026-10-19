[English](../../ASSETS.md) | 简体中文

# 资源与致谢

## 上游平台

本应用使用 [FoloToy/ai-passport](https://github.com/FoloToy/ai-passport) 的板级支持与参考源码，根目录 [LICENSE](../../../LICENSE) 保留原始 MIT 许可证与版权声明。

依赖来源与固定版本记录在 `dependencies.lock`。这些组件由 ESP-IDF 单独解析，各自的上游许可证继续适用。

## 图片

`assets/images/moon-bowl-source.png` 根据创作者确认的满月与金色颂钵概念，于 2026 年 10 月 17 日使用 OpenAI 图像生成工具创作。钵面圆形图案为示意，未核实为官方标志，也不代表品牌背书。

`moon-bowl-240.png` 和 `moon_background.c` 是设备使用的 240×320 RGB565 衍生资源，嵌入图片占用 153,600 字节。该作品未使用第三方摄影照片。

## 声音

`assets/music/bowl-{C,D,E,F,G,A,B}.wav` 与 `moon-bowls.pcm` 是为本应用创作、可复现的原创模态合成声音，按仓库 MIT 许可证提供，未使用外部录音。

每调为 16 kHz、单声道、16 位有符号小端 PCM，长度八秒；完整音库为 1,792,000 字节。可使用 `tools/generate_moon_audio.py` 重新生成。

## 字体

[Noto Sans CJK SC Regular](https://github.com/notofonts/noto-cjk/blob/main/Sans/OTF/SimplifiedChinese/NotoSansCJKsc-Regular.otf) 采用 SIL Open Font License 1.1，原许可保留在 [NotoSansCJK-LICENSE.txt](../../../assets/fonts/NotoSansCJK-LICENSE.txt)。

生成的 `moon_font_{12,14,18,28}.c` 为本应用的字形子集。原字体保留用于可复现生成，不会整份嵌入固件；生成的字体衍生资源仍受字体自身许可证约束。
