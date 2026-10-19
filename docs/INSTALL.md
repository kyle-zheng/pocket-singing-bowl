English | [简体中文](i18n/zh-CN/INSTALL.md)

# Install Pocket Singing Bowl v0.1.8

This complete merged image is for FoloToy AI Passport (ESP32-C3, 8 MB flash).

For browser-guided installation, open the community project: https://ai-passport.folotoy.cn/plays/1384/

For manual installation, connect the powered device with a USB data cable, verify the checksum in SHA256SUMS.txt, and use Espressif's esptool. Replace YOUR_SERIAL_PORT with the port belonging to your FoloToy AI Passport:

```sh
python3 -m esptool --chip esp32c3 --port YOUR_SERIAL_PORT write_flash 0x0 pocket-singing-bowl-v0.1.8-full.bin
```

This writes a complete merged image from offset zero, replacing the current application and restoring default settings. It does not require a separate full-chip erase. Save anything you need from the previous application first.

On first startup, the app waits silently in manual mode with B/crown, 50% volume, a six-second interval, a three-minute automatic session, and automatic sleep off. The device interface is in Simplified Chinese. Press OK once to play; hold OK for automatic playback; double-press OK for settings. See the [User Guide](USER_GUIDE.md) for the complete controls.
