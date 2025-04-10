# Tourbox Neo Linux 驱动程序详细文档

这是 Tourbox Neo Linux 驱动程序的详细文档，提供了完整的安装、配置和使用指南。

## 目录

- [功能特点](#功能特点)
- [系统要求](#系统要求)
- [编译和安装](#编译和安装)
- [运行](#运行)
- [配置](#配置)
- [按键对照表](#按键对照表)
- [故障排除](#故障排除)
- [开发者文档](#开发者文档)

## 功能特点

- 支持根据当前活动窗口自动切换按键映射
- 使用 JSON 配置文件，易于自定义
- 支持所有 Tourbox Neo 按键和旋钮
- 与 Hyprland 窗口管理器集成
- 支持键盘按键、鼠标移动和点击等多种输入模拟
- 低延迟，高响应度的输入处理
- 支持特殊映射，如鼠标移动和滚轮模拟

## 系统要求

- Linux 系统（推荐 Arch Linux 或基于 Arch 的发行版）
- Hyprland 窗口管理器（用于窗口识别功能）
- nlohmann_json 库（用于 JSON 配置解析）
- 适当的权限来访问串口设备和创建虚拟输入设备

## 编译和安装

### 依赖项

- C++23 兼容的编译器
- CMake (>= 3.10)
- nlohmann/json 库

在 Arch Linux 上安装依赖：

```bash
sudo pacman -S gcc cmake nlohmann-json
```

在 Ubuntu/Debian 上安装依赖：

```bash
sudo apt install build-essential cmake nlohmann-json3-dev
```

### 构建

```bash
# 克隆仓库
git clone https://github.com/yourusername/Tourbox_Neo_Linux_Driver.git
cd Tourbox_Neo_Linux_Driver

# 创建构建目录并构建
mkdir -p build
cd build
cmake ..
make

# 安装（可选）
sudo make install
```

### 清理构建

如果需要清理构建并重新开始：

```bash
rm -rf build
mkdir -p build
cd build
cmake ..
make
```

## 运行

```bash
# 如果已安装
sudo tourbox_driver /dev/ttyUSB0  # 替换为您的设备路径

# 如果未安装，从构建目录运行
cd build
sudo ./tourbox_driver /dev/ttyUSB0  # 替换为您的设备路径
```

### 查找设备路径

要查找设备路径，可以使用：

```bash
# 列出所有 USB 串口设备
ls -l /dev/ttyUSB*

# 或者查看设备连接信息
dmesg | grep tty
```

### 权限设置

为了避免每次都使用 sudo，您可以将用户添加到适当的组：

```bash
# 添加用户到 input 组（用于访问 uinput）
sudo usermod -a -G input $USER

# 添加用户到 dialout 组（用于访问串口设备）
sudo usermod -a -G dialout $USER

# 创建 udev 规则（可选，但推荐）
sudo tee /etc/udev/rules.d/99-tourbox.rules > /dev/null << 'EOF'
SUBSYSTEM=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="5750", MODE="0666", GROUP="input"
KERNEL=="uinput", MODE="0660", GROUP="input", OPTIONS+="static_node=uinput"
EOF

# 重新加载 udev 规则
sudo udevadm control --reload-rules
sudo udevadm trigger
```

注意：您需要注销并重新登录，或重启系统，使组成员身份更改生效。

## 配置

首次运行时，程序会自动创建默认配置文件：`~/.config/tourbox/config.json`

您可以编辑此配置文件来自定义按键映射和窗口规则。

### 配置文件格式

```json
{
  "presets": {
    "default": {
      "81": "KEY_MUTE",
      "49": "KEY_SCROLLUP",
      "09": "KEY_SCROLLDOWN",
      "8A": "KEY_PAUSE",
      "82": "KEY_ENTER",
      "A2": "BTN_LEFT",
      "A3": "BTN_RIGHT",
      "83": "KEY_DOT",
      "80": "KEY_MENU",
      "90": "REL_Y_NEG",
      "93": "REL_X_POS",
      "91": "REL_Y_POS",
      "92": "REL_X_NEG",
      "4F": "KEY_VOLUMEUP",
      "0F": "KEY_VOLUMEDOWN",
      "B8": "KEY_SPACE",
      "44": "KEY_BRIGHTNESSUP",
      "04": "KEY_BRIGHTNESSDOWN",
      "B7": "KEY_TAB",
      "AA": "KEY_ESC"
    },
    "gimp": {
      "81": "KEY_LEFTCTRL",
      "49": "KEY_KPPLUS",
      "09": "KEY_KPMINUS",
      "8A": "KEY_SPACE",
      "82": "KEY_B",
      "A2": "BTN_LEFT",
      "A3": "BTN_RIGHT",
      "83": "KEY_E",
      "80": "KEY_L",
      "90": "REL_Y_NEG",
      "93": "REL_X_POS",
      "91": "REL_Y_POS",
      "92": "REL_X_NEG",
      "4F": "KEY_LEFTBRACE",
      "0F": "KEY_RIGHTBRACE",
      "B8": "KEY_X",
      "44": "KEY_LEFTSHIFT",
      "04": "KEY_LEFTALT",
      "B7": "KEY_Z",
      "AA": "KEY_ESC"
    }
  },
  "window_rules": [
    {
      "class": "Gimp",
      "preset": "gimp"
    },
    {
      "class": "Blender",
      "preset": "blender"
    },
    {
      "title": "Visual Studio Code",
      "preset": "vscode"
    }
  ]
}
```

### 配置说明

- **presets**: 定义不同的按键映射预设
  - 每个预设包含按键代码到键名的映射
  - 按键代码使用十六进制格式，如 `"81"` 表示侧键按下时的代码
  - 键名使用 Linux 内核定义的标准键名，如 `"KEY_MUTE"`

- **window_rules**: 定义窗口匹配规则
  - **class**: 窗口类名（可选）
  - **title**: 窗口标题（可选，支持部分匹配）
  - **preset**: 要应用的预设名称

### 特殊键值

鼠标移动使用特殊键值：

- `REL_X_POS`: 鼠标右移
- `REL_X_NEG`: 鼠标左移
- `REL_Y_POS`: 鼠标下移
- `REL_Y_NEG`: 鼠标上移

## 按键对照表

按键映射名称来自: https://github.com/torvalds/linux/blob/master/include/uapi/linux/input-event-codes.h

| 按键名称   | 扫描码 (十六进制) | 默认映射            |
|-----------|-----------------|--------------------|
| 滚轮上                            | 49                 | KEY_SCROLLUP                  |
| 滚轮下                            | 09                 | KEY_SCROLLDOWN                |
| 滚轮 按下/释放                         | 0a:8a              | KEY_PAUSE                     |
| 侧键 按下/释放                             | 01:81              | KEY_MUTE                      |
| 横键 按下/释放                             | 02:82              | KEY_ENTER                     |
| C1 按下/释放                               | 22:a2              | BTN_LEFT (Mouse left click)   |
| C2 按下/释放                               | 23:a3              | BTN_RIGHT (Mouse right click) |
| 短键 按下/释放                             | 03:83              | KEY_DOT                       |
| 长键 按下/释放                             | 00:80              | KEY_MENU                      |
| 上 按下/释放                       | 10:90              | Mouse up                      |
| 右 按下/释放                       | 13:93              | Mouse right                   |
| 下 按下/释放               | 11:91              | Mouse down                    |
| 左 按下/释放                                | 12:92              | Mouse left                    |
| 转盘顺时针                        | 4f                 | KEY_VOLUMEUP                  |
| 转盘逆时针                        | 0f                 | KEY_VOLUMEDOWN                |
| 转盘 按下/释放                          | 38:b8              | KEY_SPACE                     |
| 旋钮顺时针                        | 44                 | KEY_BRIGHTNESSUP              |
| 旋钮逆时针                        | 04                 | KEY_BRIGHTNESSDOWN            |
| 旋钮 按下/释放                          | 37:b7              | KEY_TAB                       |
| Tour 按下/释放                              | 2a:aa              | KEY_ESC                       |

![annotated version](./images/tourbox_neo_annotated.jpg)

## 串口设置

驱动程序使用以下串口设置：

```
*** 波特率: 115200
*** 流控制: 无
*** 校验位: 无
*** 数据位: 8
*** 停止位: 1
*** DTR: 启用
*** RTS: 启用
```

## 故障排除

### 权限问题

如果遇到权限问题，确保您有权限访问串口设备和 uinput：

```bash
sudo usermod -a -G dialout $USER  # 添加用户到 dialout 组以访问串口，有的发行版是 uucp 组
sudo usermod -a -G input $USER    # 添加用户到 input 组以访问 uinput
```

修改后需要重新登录才能生效。

### 找不到设备

如果找不到 Tourbox Neo 设备，可以尝试：

```bash
dmesg | grep tty      # 查看系统识别的串口设备
lsusb                 # 列出所有 USB 设备
```

### Hyprland 集成问题

确保 Hyprland 的 IPC 接口可用：

```bash
hyprctl activewindow  # 测试 Hyprland 命令是否正常工作
```

## 贡献

欢迎提交 Pull Request 和 Issue！
