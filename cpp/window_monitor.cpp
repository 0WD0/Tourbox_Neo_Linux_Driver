#include "window_monitor.hpp"
#include <cstdio>
#include <stdexcept>

WindowMonitor::WindowMonitor() : m_running(false) {}

WindowMonitor::~WindowMonitor() {
    stop();
}

// 启动窗口监控线程
void WindowMonitor::start() {
    if (m_running) {
        return;
    }

    m_running = true;
    m_thread = std::thread(&WindowMonitor::monitorThread, this);
}

// 停止窗口监控线程
void WindowMonitor::stop() {
    if (!m_running) {
        return;
    }

    m_running = false;
    if (m_thread.joinable()) {
        m_thread.join();
    }
}

// 获取当前窗口信息
WindowInfo WindowMonitor::getCurrentWindow() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_currentWindow;
}

// 执行命令并获取输出
std::string WindowMonitor::execCommand(const std::string& cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);

    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}

// 从 Hyprland 获取活动窗口信息
WindowInfo WindowMonitor::getHyprlandActiveWindow() {
    WindowInfo info;

    try {
        // 尝试使用 hyprctl 获取活动窗口信息
        std::string output = execCommand("hyprctl activewindow -j");

        // 简单解析 JSON 输出
        // 注意：这是一个简化的解析，生产环境应该使用 JSON 库
        std::string classStr = "\"class\": \"";
        std::string titleStr = "\"title\": \"";

        auto parseField = [&output](const std::string& field) -> std::string {
            size_t pos = output.find(field);
            if (pos != std::string::npos) {
                pos += field.length();
                size_t endPos = output.find("\"", pos);
                if (endPos != std::string::npos) {
                    return output.substr(pos, endPos - pos);
                }
            }
            return "";
        };

        info.windowClass = parseField(classStr);
        info.windowTitle = parseField(titleStr);
    } catch (const std::exception& e) {
        std::cerr << "获取 Hyprland 窗口信息失败: " << e.what() << std::endl;
    }

    return info;
}

// 监控线程主函数
void WindowMonitor::monitorThread() {
    while (m_running) {
        try {
            // 获取当前活动窗口
            WindowInfo newWindow = getHyprlandActiveWindow();

            // 如果窗口信息有变化，更新当前窗口
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                if (newWindow.windowClass != m_currentWindow.windowClass || 
                    newWindow.windowTitle != m_currentWindow.windowTitle) {

                    std::cout << "窗口切换: " << newWindow.windowClass 
                        << " - " << newWindow.windowTitle << std::endl;

                    m_currentWindow = newWindow;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "窗口监控线程异常: " << e.what() << std::endl;
        }

        // 每秒检查一次窗口变化
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
