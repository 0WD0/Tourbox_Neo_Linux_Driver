cmake_minimum_required(VERSION 3.10)

# 确保二进制文件输出到构建目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(TourboxNeoDriver VERSION 0.2)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 nlohmann_json 库
find_package(nlohmann_json REQUIRED)

# 添加 cpp 子目录
add_subdirectory(cpp)

# 安装目标
install(TARGETS tourbox_driver DESTINATION bin)

# 创建一个示例配置文件
set(CONFIG_DIR ${CMAKE_CURRENT_BINARY_DIR}/config)
file(MAKE_DIRECTORY ${CONFIG_DIR})

# 示例配置文件内容
file(WRITE ${CONFIG_DIR}/config.json.example
"{
  \"presets\": {
    \"default\": {
      \"81\": \"KEY_MUTE\",
      \"49\": \"KEY_SCROLLUP\",
      \"09\": \"KEY_SCROLLDOWN\",
      \"8A\": \"KEY_PAUSE\",
      \"82\": \"KEY_ENTER\",
      \"A2\": \"BTN_LEFT\",
      \"A3\": \"BTN_RIGHT\",
      \"83\": \"KEY_DOT\",
      \"80\": \"KEY_MENU\",
      \"90\": \"REL_Y_NEG\",
      \"93\": \"REL_X_POS\",
      \"91\": \"REL_Y_POS\",
      \"92\": \"REL_X_NEG\",
      \"4F\": \"KEY_VOLUMEUP\",
      \"0F\": \"KEY_VOLUMEDOWN\",
      \"B8\": \"KEY_SPACE\",
      \"44\": \"KEY_BRIGHTNESSUP\",
      \"04\": \"KEY_BRIGHTNESSDOWN\",
      \"B7\": \"KEY_TAB\",
      \"AA\": \"KEY_ESC\"
    },
    \"gimp\": {
      \"81\": \"KEY_LEFTCTRL\",
      \"49\": \"KEY_KPPLUS\",
      \"09\": \"KEY_KPMINUS\",
      \"8A\": \"KEY_SPACE\",
      \"82\": \"KEY_B\",
      \"A2\": \"BTN_LEFT\",
      \"A3\": \"BTN_RIGHT\",
      \"83\": \"KEY_E\",
      \"80\": \"KEY_L\",
      \"90\": \"REL_Y_NEG\",
      \"93\": \"REL_X_POS\",
      \"91\": \"REL_Y_POS\",
      \"92\": \"REL_X_NEG\",
      \"4F\": \"KEY_LEFTBRACE\",
      \"0F\": \"KEY_RIGHTBRACE\",
      \"B8\": \"KEY_X\",
      \"44\": \"KEY_LEFTSHIFT\",
      \"04\": \"KEY_LEFTALT\",
      \"B7\": \"KEY_Z\",
      \"AA\": \"KEY_ESC\"
    }
  },
  \"window_rules\": [
    {
      \"class\": \"Gimp\",
      \"preset\": \"gimp\"
    }
  ]
}"
)

# 安装示例配置文件
install(FILES ${CONFIG_DIR}/config.json.example DESTINATION share/tourbox)

# 添加 README 说明
set(README_DIR ${CMAKE_CURRENT_BINARY_DIR})
file(WRITE ${README_DIR}/README.md
"# Tourbox Neo Linux 驱动

这是一个支持 Hyprland 窗口感知的 Tourbox Neo 驱动程序，可以根据当前活动窗口自动切换按键映射。

## 编译和安装

```bash
mkdir build
cd build
cmake ..
make
sudo make install
```

## 运行

```bash
sudo tourbox_driver /dev/ttyUSB0  # 替换为您的设备路径
```

## 配置

首次运行时，程序会自动创建默认配置文件：`~/.config/tourbox/config.json`

您可以编辑此配置文件来自定义按键映射和窗口规则。

### 配置文件格式

```json
{
  \"presets\": {
    \"default\": {
      \"81\": \"KEY_MUTE\",
      // 更多按键映射
    },
    \"gimp\": {
      \"81\": \"KEY_LEFTCTRL\",
      // 针对 GIMP 的映射
    }
  },
  \"window_rules\": [
    {\"class\": \"Gimp\", \"preset\": \"gimp\"}
  ]
}
```

- 按键代码使用十六进制格式，如 `\"81\"` 表示侧键按下时的代码
- 键名使用 Linux 内核定义的标准键名，如 `\"KEY_MUTE\"`
- 窗口规则根据窗口类名和标题匹配，并应用相应的预设

## 按键对照表

| 按键名称 | 扫描码 (十六进制) |
|---------|-----------------|
| 侧键     | 81              |
| 滚轮上   | 49              |
| 滚轮下   | 09              |
| 滚轮单击 | 8A              |
| 横键     | 82              |
| C1      | A2              |
| C2      | A3              |
| 短键     | 83              |
| 长键     | 80              |
| 上       | 90              |
| 右       | 93              |
| 下       | 91              |
| 左       | 92              |
| 转盘顺时针 | 4F             |
| 转盘逆时针 | 0F             |
| 转盘单击  | B8              |
| 旋钮顺时针 | 44             |
| 旋钮逆时针 | 04             |
| 旋钮单击  | B7              |
| Tour    | AA              |
"
)

# 安装 README
install(FILES ${README_DIR}/README.md DESTINATION share/doc/tourbox)
