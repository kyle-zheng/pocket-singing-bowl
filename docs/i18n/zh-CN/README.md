[English](../../../README.md) | 简体中文

<h1 align="center">口袋颂钵</h1>

<p align="center">一轮满月，一只金色的钵，把片刻宁静放进口袋。</p>

<p align="center">
  <a href="https://github.com/kyle-zheng/pocket-singing-bowl/releases/tag/v0.1.8"><img src="../../../assets/badges/version.svg" alt="Version: v0.1.8"></a>
  <a href="https://github.com/kyle-zheng/pocket-singing-bowl/blob/main/docs/USER_GUIDE.md"><img src="../../../assets/badges/platform.svg" alt="Platform: FoloToy AI Passport"></a>
  <a href="https://github.com/kyle-zheng/pocket-singing-bowl/blob/main/docs/USER_GUIDE.md"><img src="../../../assets/badges/sound.svg" alt="Sound: 7 tones"></a>
  <a href="https://github.com/kyle-zheng/pocket-singing-bowl/blob/main/docs/USER_GUIDE.md"><img src="../../../assets/badges/mode.svg" alt="Mode: Offline"></a>
  <a href="https://github.com/kyle-zheng/pocket-singing-bowl/blob/main/LICENSE"><img src="../../../assets/badges/license.svg" alt="License: MIT"></a>
</p>

口袋颂钵（Pocket Singing Bowl）是为 [FoloToy AI Passport](https://github.com/FoloToy/ai-passport) 制作的离线颂钵应用，灵感来自尼泊尔喜马拉雅颂钵。我将七种原创合成音调对应七脉轮，做成七个可切换的聆听主题。搭配简单的按键操作，为日常放松、专注和冥想提供一个随身的小入口。

<p align="center"><img src="../../../assets/images/pocket-singing-bowl-device-preview.png" alt="FoloToy AI Passport 机身内展示口袋颂钵界面，旁边展示满月界面细节" width="900"></p>

*AI 生成的设备示意图，展示口袋颂钵在 FoloToy AI Passport 上的界面，并非实机摄影。*

<p align="center"><img src="../../../assets/images/moon-bowl-source.png" alt="AI 生成的满月与金色颂钵示意图" width="300"></p>

[下载 v0.1.8](https://github.com/kyle-zheng/pocket-singing-bowl/releases/tag/v0.1.8) · [在线试玩](https://apsimulator.folotoy.cn/?play=1384) · [玩法社区](https://ai-passport.folotoy.cn/plays/1384/) · [使用方法](USER_GUIDE.md) · [从源码编译](BUILDING.md)

## 可以做什么

- 在 C 调海底轮至 B 调顶轮之间选择七种音调与主题。
- 短按播放一声，也可以开启自动颂钵，按自己的节奏聆听余音。
- 敲钵间隔可设为 6～60 秒，自动播放可选定时或不限时。
- 音量可调为 0%～100%，0% 为静音；长按按键可加快调节。
- 可选开启闲置十分钟后自动休眠，并以屏幕渐暗过渡；默认关闭。
- 全程离线，设置会保存。

当前设备界面为简体中文；项目文档默认英文，并提供简体中文入口。上图为 AI 生成的示意图，钵面保留最初设计采用的满月钵圆形图案。声音为原创合成音色，并非实体颂钵录音；主题和脉轮名称用于组织聆听体验，不宣称医学疗效。

## 七种音调与七脉轮

我在口袋颂钵中采用以下对应关系，每个音调都有一个聆听主题：

| 音调 | 对应脉轮 | 聆听主题 |
| --- | --- | --- |
| C 调 | 海底轮 | 安定 · 放松 |
| D 调 | 脐轮 | 感受 · 流动 |
| E 调 | 太阳轮 | 自信 · 行动 |
| F 调 | 心轮 | 接纳 · 关怀 |
| G 调 | 喉轮 | 表达 · 倾听 |
| A 调 | 眉心轮 | 专注 · 觉察 |
| B 调 | 顶轮 | 静心 · 冥想 |

## 开始聆听

1. 将固件安装到 FoloToy AI Passport，无需配网。可从上方玩法社区进入安装流程；手动刷写请查看[安装指南](INSTALL.md)。
2. 短按确认键播放一声，短按上／下键切换音调与主题。
3. 长按确认键开始自动颂钵，短按暂停或继续，再长按返回手动待机。
4. 双击确认键进入设置。短按上／下键选择或调整，短按确认键进入编辑或保存；长按确认键取消编辑，或从设置列表返回。编辑间隔和音量时，可长按上／下键快速调节。

进入设置会暂停自动颂钵；返回主页后，短按确认键继续。英文说明中的 OK、UP、DOWN 分别对应确认键、上键、下键。

## v0.1.8 默认设置

| 设置 | 默认值 |
| --- | --- |
| 音调 | B 调 · 顶轮 |
| 主题 | 静心 · 冥想 |
| 颂钵音量 | 50% |
| 敲钵间隔 | 6 秒 |
| 自动时长 | 3 分钟 |
| 自动休眠 | 关闭 |
| 开机状态 | 手动待机，不自动发声 |

普通重启会优先读取已保存的设置。完整合并镜像刷写会替换现有应用并恢复默认设置；刷写前请阅读安装说明。

## 进一步了解

| 文档 | 内容 |
| --- | --- |
| [使用方法](USER_GUIDE.md) | 按键、主题、设置与休眠行为 |
| [安装指南](INSTALL.md) | 安装完整镜像与校验文件 |
| [编译指南](BUILDING.md) | ESP-IDF 环境、固件构建与主机测试 |
| [架构说明](ARCHITECTURE.md) | 控制逻辑、音频混音与电源行为 |
| [资源与致谢](ASSETS.md) | 图片、合成音频、字体及上游来源 |
| [版本说明](RELEASE_NOTES.md) | 当前版本与用户可感知的改进 |
| [许可证](../../../LICENSE) | MIT 许可证；字体保留独立许可证 |

## 开发状态

我已经在 FoloToy AI Passport 上刷入并试听了 v0.1.8。这版默认使用 B 调顶轮、50% 音量，并保留此前的音色与混音优化；调音后，我在试听中已听不到之前的爆鸣杂音。

## 致谢

基于 FoloToy AI Passport 开源固件和板级支持代码开发，保留 [LICENSE](../../../LICENSE) 中的原始 MIT 声明。字体许可与生成图片来源见[资源与致谢](ASSETS.md)。本项目为独立社区应用。
