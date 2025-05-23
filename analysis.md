# TourBox Console 结构分析

## 概述

TourBox Console 是 TourBox Neo 设备的 Windows 控制软件，主要由 Java 编写，使用 SWT 框架构建界面。该软件允许用户配置和管理 TourBox 设备的按键映射、宏和插件功能。

## 目录结构

### 主要文件

- `TourBox Console.exe` - 主程序可执行文件 (690KB)
- `TourBox Console.json` - 程序配置文件，定义了 Java 运行环境和主类
- `TourBoxConsole_win.jar` - 控制台程序 JAR 包 (16KB)
- `TourBox_win.jar` - 主程序 JAR 包 (38MB)
- `tourbox.db` - 配置数据库，存储用户设置和预设
- `reg_x64.dll` - 64位 Windows 注册表操作库
- `runtime.cfg` - 运行时配置文件
- `uninst.exe` - 卸载程序

### 主要目录

#### drivers 目录
包含多种 Windows 驱动程序，用于不同版本的 Windows 系统识别 TourBox 设备：
- AT_CDC 驱动 (多个版本，适用于 Windows 7/8，32/64位)
- CP210x 驱动 (适用于 Windows 7/8/10，32/64位)
- GVM_CDC 和 SCCC_CDC 驱动
- `devcon.exe` - Windows 设备管理工具

#### jre 目录
包含 Java 运行环境，使软件能够在没有安装 Java 的系统上运行：
- `bin` - Java 可执行文件
- `lib` - Java 库文件
- 许可证和第三方声明文件

#### macro 目录
包含宏功能相关文件：
- `macro.html` - 宏编辑界面 (464KB)
- `connect_img.html` - 连接图像界面
- `assets` - 资源文件目录

#### plugin 目录
包含与其他软件集成的插件：
- `TourBox.acsrf` - Adobe After Effects 插件
- `TourBox.bundle` - 可能是 Final Cut Pro 插件
- `Tourbox.lrplugin` - Adobe Lightroom 插件
- `com.tourbox.premiere.cep` - Adobe Premiere Pro 插件

#### template 目录
包含预设模板：
- `jytemp` - 可能是 Photoshop 模板
- `pstemp` - 可能是 Premiere 模板

#### win64libs 目录
包含 Windows 64位库文件，支持软件运行

## 配置文件分析

### TourBox Console.json
```json
{
  "jrePath": "jre",
  "classPath": [
    "TourBoxConsole_win.jar"
  ],
  "mainClass": "com.tourbox.ui.launch.TourBoxMain",
  "useZgcIfSupportedOs": false,
  "vmArgs": [
    "-Dfile.encoding=UTF-8",
    "-Dswt.library.path=./win64libs",
    "-Dorg.eclipse.swt.browser.DefaultType=chromium,edge",
    "-Dswt.autoScale=exact"
  ]
}
```

该配置文件指定了：
- Java 运行环境路径
- 类路径包含 TourBoxConsole_win.jar
- 主类为 com.tourbox.ui.launch.TourBoxMain
- JVM 参数，包括 SWT 库路径和浏览器设置

### tourbox.db
数据库文件使用 XML 格式存储配置数据，包含版本信息和二进制编码的配置数据。这些数据可能包含按键映射、宏定义和用户预设。文件结构以 `<TBDB>` 标签开始，包含版本信息和二进制编码的配置数据。

## JAR 包分析

### TourBoxConsole_win.jar
这是一个小型启动器 JAR 包 (16KB)，包含以下主要组件：
- `com.tourbox.ui.launch.TourBoxMain` - 主类，负责启动应用程序
- `com.tourbox.ui.launch.FileUtil` - 文件操作工具类
- `com.tourbox.ui.launch.GlobalConfig` - 全局配置管理
- `com.tourbox.ui.launch.OsCheck` - 操作系统检测工具

### TourBox_win.jar
这是主程序 JAR 包 (38MB)，包含大量被混淆的类（使用 `oO0[数字]oO0` 格式命名）。主要包括：

1. **产品模块**：
   - `com.tourbox.product.TB004` - 可能是 TourBox Neo 产品代码
   - 包含多种颜色主题和元素资源

2. **插件模块**：
   - 支持多种创意软件的插件：Photoshop、Lightroom、Premiere Pro、Final Cut、DaVinci Resolve 等
   - 每个插件都有标准和精简版本 (lite)

3. **用户界面模块**：
   - `com.tourbox.ui` - 包含界面组件
   - 支持多语言：英语、中文（简体和繁体）、日语、韩语、德语、法语、意大利语、西班牙语、俄语

4. **通信模块**：
   - `com.tourbox.serialportutil` - 串口通信工具
   - 使用 `org.usb4java` 库进行 USB 通信
   - 使用 JNA (Java Native Access) 进行底层硬件访问

## 技术架构

1. **前端**：
   - 使用 Java SWT 框架构建本地 GUI
   - 支持多语言和主题定制

2. **后端**：
   - Java 应用程序处理设备通信和配置管理
   - 使用混淆技术保护核心代码

3. **通信层**：
   - 使用 USB4Java 库进行 USB 通信
   - 使用 NRJavaSerial 库进行串行通信
   - 支持多种操作系统和硬件架构

4. **插件系统**：
   - 为各种创意软件提供集成插件
   - 支持宏功能和自定义按键映射

5. **驱动层**：
   - 提供多种 Windows 驱动程序支持设备识别
   - 使用 CDC (Communication Device Class) 和 CP210x 串行驱动

## 设备通信分析

TourBox Console 使用多种方式与设备通信：

1. **USB 通信**：
   - 使用 `org.usb4java` 库进行 USB 设备识别和通信
   - 支持多种操作系统和硬件架构的原生库
   - 可能使用 HID (Human Interface Device) 协议

2. **串行通信**：
   - 使用 `NRJavaSerial` 库进行串行通信
   - 支持多种操作系统和硬件架构

3. **设备状态管理**：
   - `com.tourbox.model.DeviceStatus` 类管理设备状态
   - 可能使用 BLE (Bluetooth Low Energy) 进行无线通信

4. **驱动支持**：
   - 使用 AT_CDC 和 CP210x 驱动程序
   - 支持多种 Windows 版本和架构

## Linux 驱动开发考虑

基于此分析，Linux 驱动开发应考虑：

1. **设备通信协议**：
   - 使用 libusb 或 hidapi 实现与 TourBox 设备的通信
   - 需要分析 USB 通信协议，可能需要抓包分析
   - 考虑使用 USB4Java 库的 Linux 版本作为参考

2. **配置存储**：
   - 可以参考 tourbox.db 的 XML 结构设计 Linux 配置存储
   - 考虑使用 SQLite 或 XML 格式存储用户配置和预设

3. **插件架构**：
   - 为 Linux 创意软件（如 GIMP、Krita、Inkscape、Kdenlive）开发插件
   - 考虑使用 D-Bus 或 IPC 进行应用程序间通信

4. **多预设支持**：
   - 实现类似的预设管理功能
   - 支持按应用程序自动切换预设

5. **用户界面**：
   - 考虑使用 Qt 或 GTK 开发 Linux 友好的界面
   - 支持多语言和主题定制

## 未来开发方向

根据之前的记忆和分析结果，未来开发方向包括：

1. **多预设支持**：
   - 实现类似 Windows 版本的多预设管理
   - 支持预设导入/导出功能

2. **设备预设切换**：
   - 开发设备预设切换和切换工具
   - 支持快捷键或设备按钮触发预设切换

3. **窗口焦点自动预设**：
   - 添加 Hyprland 窗口焦点自动预设选择
   - 根据活动窗口自动切换预设配置

4. **视觉提示系统**：
   - 开发 Wayland 兼容的 WhichKey 风格视觉提示
   - 显示当前按键映射和可用操作

5. **配置工具**：
   - 构建功能强大且用户友好的配置工具
   - 提供可视化的按键映射和宏编辑界面

6. **项目结构优化**：
   - 将 'cpp' 目录重命名为 'src' 以改善项目组织
   - 修改 CMakeLists.txt 以提高模块化
   - 考虑将项目从 C++ 迁移到 Zig 编程语言，提高性能和安全性