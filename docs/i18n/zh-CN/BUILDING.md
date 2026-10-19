[English](../../BUILDING.md) | 简体中文

# 编译与测试

## 准备条件

- FoloToy AI Passport（ESP32-C3、8 MB Flash）。
- **ESP-IDF 5.5.3**，已安装 ESP32-C3 工具并激活其环境。
- Git、Python 3.10 或更新版本，以及用于主机测试的 C 编译器。

可参考乐鑫的 [ESP-IDF 入门指南](https://docs.espressif.com/projects/esp-idf/en/v5.5.3/esp32c3/get-started/index.html) 准备工具链。运行依赖已固定在 `dependencies.lock`；首次解析依赖需要联网，安装后的应用本身完全离线。

## 编译

在已激活 ESP-IDF 的终端中，从仓库根目录运行：

```sh
idf.py build
idf.py merge-bin -o build/FoloToy-AI-Passport-full.bin
python3 tools/verify_firmware.py build
```

`sdkconfig.defaults` 已指定 ESP32-C3。CMake 项目保留上游构建产物前缀，因此合并镜像名为 `FoloToy-AI-Passport-full.bin`。

## 安装

最方便的入口是[玩法社区](https://ai-passport.folotoy.cn/plays/1384/)，也可以先[在线试玩](https://apsimulator.folotoy.cn/?play=1384)。发布镜像的手动安装步骤见[安装指南](INSTALL.md)。

本地开发构建可使用：

```sh
idf.py -p YOUR_SERIAL_PORT flash
```

将 `YOUR_SERIAL_PORT` 替换为设备对应的串口。该命令写入构建的各个镜像；发布的完整合并镜像应按其安装说明从偏移零写入。完整刷写可能将设置恢复为默认值；替换其他应用前请保存需要的数据。

## 主机测试

```sh
python3 tools/run_host_tests.py
```

测试覆盖应用状态机、间隔与音量编辑、静音、自动播放、混音边界、唤醒保护、电量过滤、生成资源和通用板级支持逻辑。无需连接设备，只需 Python 标准库和 C 编译器。

## 重新生成资源（可选）

仓库已包含生成资源，直接编译不必重新生成。需要重新生成时，可在虚拟环境中安装 Pillow、numpy 和 fonttools：

```sh
python3 -m venv .venv
. .venv/bin/activate
python3 -m pip install Pillow numpy fonttools
python3 tools/generate_moon_assets.py
```

仅重新生成音频：

```sh
python3 tools/generate_moon_audio.py
```

生成过程会改变对应文件，提交前请审阅差异。

## 验证范围

主机测试和编译通过不能证明扬声器听感、电量准确性或休眠电流。修改音频或电源行为后，应在实机上测试全部音调、连续按键、自动播放、静音、设置保存及反复休眠唤醒。
