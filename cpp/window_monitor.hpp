#ifndef WINDOW_MONITOR_HPP
#define WINDOW_MONITOR_HPP

#include <string>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>
#include <array>
#include <memory>

// 窗口信息结构
struct WindowInfo {
    std::string windowClass;
    std::string windowTitle;
};

class WindowMonitor {
public:
    WindowMonitor();
    ~WindowMonitor();

    // 启动窗口监控线程
    void start();

    // 停止窗口监控线程
    void stop();

    // 获取当前窗口信息
    WindowInfo getCurrentWindow();

private:
    // 执行命令并获取输出
    std::string execCommand(const std::string& cmd);

    // 从 Hyprland 获取活动窗口信息
    WindowInfo getHyprlandActiveWindow();

    // 监控线程主函数
    void monitorThread();

    std::thread m_thread;
    std::atomic<bool> m_running;
    std::mutex m_mutex;
    WindowInfo m_currentWindow;
};

#endif // WINDOW_MONITOR_HPP
