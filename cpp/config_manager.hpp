#ifndef CONFIG_MANAGER_HPP
#define CONFIG_MANAGER_HPP

#include <string>
#include <map>
#include <vector>
#include <fstream>
#include <iostream>
#include <linux/input-event-codes.h>
#include <nlohmann/json.hpp>
#include "uinput_helper.hpp"

using json = nlohmann::json;

// 定义窗口规则结构
struct WindowRule {
    std::string windowClass;
    std::string windowTitle;
    std::string presetName;

    bool matches(const std::string& activeClass, const std::string& activeTitle) const;
};

// 按键映射类型
using KeyMapping = std::map<uint8_t, int>;

class ConfigManager {
public:
    ConfigManager(const std::string& configPath = "~/.config/tourbox/config.json");
    ~ConfigManager() = default;

    // 加载配置文件
    bool loadConfig();

    // 根据窗口信息获取按键映射
    int getKeyMapping(uint8_t buttonCode, const std::string& windowClass, const std::string& windowTitle);

    // 获取所有需要注册的键码
    std::vector<int> getAllKeyCodes() const;

    // 创建默认配置文件
    void createDefaultConfig();

private:
    // 加载默认按键映射
    void loadDefaultMappings();

    std::string m_configPath;
    std::string m_activePreset;
    std::map<std::string, KeyMapping> m_presets;
    std::vector<WindowRule> m_windowRules;
    std::map<std::string, int> m_keyNameMap;
};

#endif // CONFIG_MANAGER_HPP
