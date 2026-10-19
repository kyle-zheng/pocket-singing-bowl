[English](../../INSTALL.md) | 简体中文

# 安装口袋颂钵 v0.1.8

此完整合并镜像适用于 FoloToy AI Passport（ESP32-C3、8 MB Flash）。

浏览器引导安装可进入[玩法社区](https://ai-passport.folotoy.cn/plays/1384/)。

手动安装时，请使用 USB 数据线连接已开机的设备，核对 `SHA256SUMS.txt` 中的校验值，并使用乐鑫 esptool。将 `YOUR_SERIAL_PORT` 替换为你的 AI Passport 对应串口：

```sh
python3 -m esptool --chip esp32c3 --port YOUR_SERIAL_PORT write_flash 0x0 pocket-singing-bowl-v0.1.8-full.bin
```

该命令从偏移零写入完整合并镜像，替换当前应用并恢复默认设置，无需另行执行整片擦除。请先保存旧应用中需要的数据。

首次开机为手动待机，不自动发声；默认 B 调顶轮、50% 音量、六秒间隔、三分钟自动时长，自动休眠关闭。设备界面为简体中文。短按确认键播放，长按开始自动颂钵，双击进入设置；完整操作见[使用方法](USER_GUIDE.md)。
