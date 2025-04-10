#include "config_manager.hpp"
#include "uinput_helper.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

// WindowRule 方法实现
bool WindowRule::matches(const std::string& activeClass, const std::string& activeTitle) const {
    if (!windowClass.empty() && windowClass != activeClass) {
        return false;
    }
    if (!windowTitle.empty() && activeTitle.find(windowTitle) == std::string::npos) {
        return false;
    }
    return true;
}

// ConfigManager 构造函数
ConfigManager::ConfigManager(const std::string& configPath)
    : m_configPath(configPath), m_activePreset("default") {
    // 展开 ~ 到用户主目录
    if (m_configPath.find("~") == 0) {
        const char* homeDir = getenv("HOME");
        if (homeDir) {
            m_configPath.replace(0, 1, homeDir);
        }
    }

    loadDefaultMappings();
    loadConfig();
}

// 加载配置文件
bool ConfigManager::loadConfig() {
    try {
        std::ifstream configFile(m_configPath);
        if (!configFile.is_open()) {
            std::cerr << "无法打开配置文件，使用默认配置" << std::endl;
            createDefaultConfig();
            return false;
        }

		std::cerr<<"正在加载配置文件: "<<m_configPath<<std::endl;

        json config;
        configFile >> config;

        // 加载预设
        for (auto& [presetName, mappings] : config["presets"].items()) {
            KeyMapping keyMapping;

            for (auto& [buttonCode, keyCode] : mappings.items()) {
                // 将十六进制字符串转换为整数
                uint8_t code = std::stoi(buttonCode, nullptr, 16);

                // 如果键值是字符串，查找对应的键码
                if (keyCode.is_string()) {
                    std::string keyName = keyCode;
                    if (m_keyNameMap.find(keyName) != m_keyNameMap.end()) {
                        keyMapping[code] = m_keyNameMap[keyName];
                    } else {
                        std::cerr << "未知键名: " << keyName << std::endl;
                    }
                } else if (keyCode.is_number()) {
                    keyMapping[code] = keyCode;
                }
            }

            m_presets[presetName] = keyMapping;
        }

        // 加载窗口规则
        m_windowRules.clear();
        for (auto& rule : config["window_rules"]) {
            WindowRule windowRule;
            windowRule.windowClass = rule.value("class", "");
            windowRule.windowTitle = rule.value("title", "");
            windowRule.presetName = rule.value("preset", "default");

            m_windowRules.push_back(windowRule);
        }

        return true;
    } catch (const std::exception& e) {
        std::cerr << "加载配置文件失败: " << e.what() << std::endl;
        return false;
    }
}

// 根据窗口信息获取按键映射
int ConfigManager::getKeyMapping(uint8_t buttonCode, const std::string& windowClass, const std::string& windowTitle) {
    // 查找匹配的窗口规则
    std::string presetName = "default";
    for (const auto& rule : m_windowRules) {
        if (rule.matches(windowClass, windowTitle)) {
            presetName = rule.presetName;
            break;
        }
    }

    // 如果预设发生变化，输出提示
    if (presetName != m_activePreset) {
        std::cout << "切换到预设: " << presetName << std::endl;
        m_activePreset = presetName;
    }

    // 查找预设中的按键映射
    if (m_presets.find(presetName) != m_presets.end()) {
        const auto& mapping = m_presets[presetName];
        if (mapping.find(buttonCode) != mapping.end()) {
            return mapping.at(buttonCode);
        }
    }

    // 如果没有找到映射，使用默认预设
    if (presetName != "default" && m_presets.find("default") != m_presets.end()) {
        const auto& defaultMapping = m_presets["default"];
        if (defaultMapping.find(buttonCode) != defaultMapping.end()) {
            return defaultMapping.at(buttonCode);
        }
    }

    // 如果还是没有找到，返回0（无映射）
    return 0;
}

// 获取所有需要注册的键码
std::vector<int> ConfigManager::getAllKeyCodes() const {
    std::vector<int> keyCodes;
    std::map<int, bool> uniqueKeyCodes;

    // 收集所有预设中使用的键码
    for (const auto& [presetName, mapping] : m_presets) {
        for (const auto& [buttonCode, keyCode] : mapping) {
            // 跳过已经添加过的键码
            if (uniqueKeyCodes.find(keyCode) == uniqueKeyCodes.end()) {
                uniqueKeyCodes[keyCode] = true;
                keyCodes.push_back(keyCode);
            }
        }
    }

    return keyCodes;
}

// 创建默认配置文件
void ConfigManager::createDefaultConfig() {
    try {
        // 确保配置目录存在
        std::filesystem::path configPath(m_configPath);
        std::filesystem::create_directories(configPath.parent_path());

        // 创建默认配置
        json config;

        // 默认预设
        config["presets"]["default"] = {
            {"80", "KEY_LEFTSHIFT"},  // 长键
            {"81", "KEY_LEFTCTRL"},   // 侧键
            {"82", "KEY_LEFTALT"},    // 横键
            {"83", "KEY_SPACE"},      // 短键
            {"4F", "KEY_BRIGHTNESSUP"},  // 转盘顺时针
            {"0F", "KEY_BRIGHTNESSDOWN"}, // 转盘逆时针
            {"90", "KEY_UP"},         // D-Pad 上
            {"91", "KEY_DOWN"},       // D-Pad 下
            {"92", "KEY_LEFT"},       // D-Pad 左
            {"93", "KEY_RIGHT"},      // D-Pad 右
            {"8A", "BTN_MIDDLE"},     // 滚轮单击
            {"49", "REL_Y_NEG"},      // 滚轮上滚动
            {"09", "REL_Y_POS"},      // 滚轮下滚动
            {"A2", "KEY_Z"},          // C1
            {"A3", "KEY_X"},          // C2
            {"44", "KEY_BRIGHTNESSUP"}, // 旋钮顺时针
            {"04", "KEY_BRIGHTNESSDOWN"}, // 旋钮逆时针
            {"B7", "KEY_TAB"},        // 旋钮单击
            {"B8", "BTN_LEFT"},       // 转盘单击
            {"AA", "KEY_ESC"}         // Tour
        };

        // GIMP预设示例
        config["presets"]["gimp"] = {
            {"80", "KEY_LEFTSHIFT"},  // 长键
            {"81", "KEY_LEFTCTRL"},   // 侧键
            {"82", "KEY_LEFTALT"},    // 横键
            {"83", "KEY_SPACE"},      // 短键
            {"4F", "KEY_EQUAL"},      // 转盘顺时针 - 放大
            {"0F", "KEY_MINUS"},      // 转盘逆时针 - 缩小
            {"90", "KEY_UP"},         // D-Pad 上
            {"91", "KEY_DOWN"},       // D-Pad 下
            {"92", "KEY_LEFT"},       // D-Pad 左
            {"93", "KEY_RIGHT"},      // D-Pad 右
            {"8A", "BTN_MIDDLE"},     // 滚轮单击
            {"49", "REL_Y_NEG"},      // 滚轮上滚动
            {"09", "REL_Y_POS"},      // 滚轮下滚动
            {"A2", "KEY_B"},          // C1 - 画笔工具
            {"A3", "KEY_E"},          // C2 - 橡皮擦工具
            {"44", "KEY_RIGHTBRACE"}, // 旋钮顺时针 - 增加画笔大小
            {"04", "KEY_LEFTBRACE"},  // 旋钮逆时针 - 减小画笔大小
            {"B7", "KEY_X"},          // 旋钮单击 - 切换前景/背景色
            {"B8", "BTN_LEFT"},       // 转盘单击
            {"AA", "KEY_ESC"}         // Tour
        };

        // Blender预设示例
        config["presets"]["blender"] = {
            {"80", "KEY_LEFTSHIFT"},  // 长键
            {"81", "KEY_LEFTCTRL"},   // 侧键
            {"82", "KEY_LEFTALT"},    // 横键
            {"83", "KEY_SPACE"},      // 短键
            {"4F", "KEY_EQUAL"},      // 转盘顺时针 - 放大
            {"0F", "KEY_MINUS"},      // 转盘逆时针 - 缩小
            {"90", "KEY_UP"},         // D-Pad 上
            {"91", "KEY_DOWN"},       // D-Pad 下
            {"92", "KEY_LEFT"},       // D-Pad 左
            {"93", "KEY_RIGHT"},      // D-Pad 右
            {"8A", "BTN_MIDDLE"},     // 滚轮单击
            {"49", "REL_Y_NEG"},      // 滚轮上滚动
            {"09", "REL_Y_POS"},      // 滚轮下滚动
            {"A2", "KEY_G"},          // C1 - 移动工具
            {"A3", "KEY_R"},          // C2 - 旋转工具
            {"44", "KEY_PAGEUP"},     // 旋钮顺时针
            {"04", "KEY_PAGEDOWN"},   // 旋钮逆时针
            {"B7", "KEY_TAB"},        // 旋钮单击 - 切换编辑模式
            {"B8", "BTN_LEFT"},       // 转盘单击
            {"AA", "KEY_ESC"}         // Tour
        };

        // 窗口规则
        config["window_rules"] = json::array();
        
        // GIMP规则
        json gimpRule;
        gimpRule["class"] = "Gimp";
        gimpRule["title"] = "";
        gimpRule["preset"] = "gimp";
        config["window_rules"].push_back(gimpRule);
        
        // Blender规则
        json blenderRule;
        blenderRule["class"] = "Blender";
        blenderRule["title"] = "";
        blenderRule["preset"] = "blender";
        config["window_rules"].push_back(blenderRule);

        // 写入配置文件
        std::ofstream configFile(m_configPath);
        configFile << std::setw(4) << config << std::endl;
        
        std::cout << "已创建默认配置文件: " << m_configPath << std::endl;
        
        // 重新加载配置
        loadConfig();
    } catch (const std::exception& e) {
        std::cerr << "创建默认配置文件失败: " << e.what() << std::endl;
    }
}

// 加载默认按键映射
void ConfigManager::loadDefaultMappings() {
    // 键名到键码的映射
    m_keyNameMap = {
        // 特殊映射
        {"REL_X_POS", REL_X_POS},
        {"REL_X_NEG", REL_X_NEG},
        {"REL_Y_POS", REL_Y_POS},
        {"REL_Y_NEG", REL_Y_NEG},

        // 鼠标按钮
        {"BTN_LEFT", BTN_LEFT},
        {"BTN_RIGHT", BTN_RIGHT},
        {"BTN_MIDDLE", BTN_MIDDLE},

        // 常用键
        {"KEY_ESC", KEY_ESC},
        {"KEY_1", KEY_1},
        {"KEY_2", KEY_2},
        {"KEY_3", KEY_3},
        {"KEY_4", KEY_4},
        {"KEY_5", KEY_5},
        {"KEY_6", KEY_6},
        {"KEY_7", KEY_7},
        {"KEY_8", KEY_8},
        {"KEY_9", KEY_9},
        {"KEY_0", KEY_0},
        {"KEY_MINUS", KEY_MINUS},
        {"KEY_EQUAL", KEY_EQUAL},
        {"KEY_BACKSPACE", KEY_BACKSPACE},
        {"KEY_TAB", KEY_TAB},
        {"KEY_Q", KEY_Q},
        {"KEY_W", KEY_W},
        {"KEY_E", KEY_E},
        {"KEY_R", KEY_R},
        {"KEY_T", KEY_T},
        {"KEY_Y", KEY_Y},
        {"KEY_U", KEY_U},
        {"KEY_I", KEY_I},
        {"KEY_O", KEY_O},
        {"KEY_P", KEY_P},
        {"KEY_LEFTBRACE", KEY_LEFTBRACE},
        {"KEY_RIGHTBRACE", KEY_RIGHTBRACE},
        {"KEY_ENTER", KEY_ENTER},
        {"KEY_LEFTCTRL", KEY_LEFTCTRL},
        {"KEY_A", KEY_A},
        {"KEY_S", KEY_S},
        {"KEY_D", KEY_D},
        {"KEY_F", KEY_F},
        {"KEY_G", KEY_G},
        {"KEY_H", KEY_H},
        {"KEY_J", KEY_J},
        {"KEY_K", KEY_K},
        {"KEY_L", KEY_L},
        {"KEY_SEMICOLON", KEY_SEMICOLON},
        {"KEY_APOSTROPHE", KEY_APOSTROPHE},
        {"KEY_GRAVE", KEY_GRAVE},
        {"KEY_LEFTSHIFT", KEY_LEFTSHIFT},
        {"KEY_BACKSLASH", KEY_BACKSLASH},
        {"KEY_Z", KEY_Z},
        {"KEY_X", KEY_X},
        {"KEY_C", KEY_C},
        {"KEY_V", KEY_V},
        {"KEY_B", KEY_B},
        {"KEY_N", KEY_N},
        {"KEY_M", KEY_M},
        {"KEY_COMMA", KEY_COMMA},
        {"KEY_DOT", KEY_DOT},
        {"KEY_SLASH", KEY_SLASH},
        {"KEY_RIGHTSHIFT", KEY_RIGHTSHIFT},
        {"KEY_KPASTERISK", KEY_KPASTERISK},
        {"KEY_LEFTALT", KEY_LEFTALT},
        {"KEY_SPACE", KEY_SPACE},
        {"KEY_CAPSLOCK", KEY_CAPSLOCK},
        {"KEY_F1", KEY_F1},
        {"KEY_F2", KEY_F2},
        {"KEY_F3", KEY_F3},
        {"KEY_F4", KEY_F4},
        {"KEY_F5", KEY_F5},
        {"KEY_F6", KEY_F6},
        {"KEY_F7", KEY_F7},
        {"KEY_F8", KEY_F8},
        {"KEY_F9", KEY_F9},
        {"KEY_F10", KEY_F10},
        {"KEY_F11", KEY_F11},
        {"KEY_F12", KEY_F12},
        {"KEY_NUMLOCK", KEY_NUMLOCK},
        {"KEY_SCROLLLOCK", KEY_SCROLLLOCK},
        {"KEY_KP7", KEY_KP7},
        {"KEY_KP8", KEY_KP8},
        {"KEY_KP9", KEY_KP9},
        {"KEY_KPMINUS", KEY_KPMINUS},
        {"KEY_KP4", KEY_KP4},
        {"KEY_KP5", KEY_KP5},
        {"KEY_KP6", KEY_KP6},
        {"KEY_KPPLUS", KEY_KPPLUS},
        {"KEY_KP1", KEY_KP1},
        {"KEY_KP2", KEY_KP2},
        {"KEY_KP3", KEY_KP3},
        {"KEY_KP0", KEY_KP0},
        {"KEY_KPDOT", KEY_KPDOT},
        {"KEY_RIGHTCTRL", KEY_RIGHTCTRL},
        {"KEY_KPSLASH", KEY_KPSLASH},
        {"KEY_RIGHTALT", KEY_RIGHTALT},
        {"KEY_HOME", KEY_HOME},
        {"KEY_UP", KEY_UP},
        {"KEY_PAGEUP", KEY_PAGEUP},
        {"KEY_LEFT", KEY_LEFT},
        {"KEY_RIGHT", KEY_RIGHT},
        {"KEY_END", KEY_END},
        {"KEY_DOWN", KEY_DOWN},
        {"KEY_PAGEDOWN", KEY_PAGEDOWN},
        {"KEY_INSERT", KEY_INSERT},
        {"KEY_DELETE", KEY_DELETE},
        {"KEY_MUTE", KEY_MUTE},
        {"KEY_VOLUMEDOWN", KEY_VOLUMEDOWN},
        {"KEY_VOLUMEUP", KEY_VOLUMEUP},
        {"KEY_PAUSE", KEY_PAUSE},
        {"KEY_SCROLLUP", KEY_SCROLLUP},
        {"KEY_SCROLLDOWN", KEY_SCROLLDOWN},
        {"KEY_MENU", KEY_MENU},
        {"KEY_BRIGHTNESSDOWN", KEY_BRIGHTNESSDOWN},
        {"KEY_BRIGHTNESSUP", KEY_BRIGHTNESSUP}
    };
}
