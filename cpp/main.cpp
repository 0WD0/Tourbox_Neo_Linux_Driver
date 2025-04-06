#include <array>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <filesystem>
#include <signal.h>
#include <stdint.h>
#include <string>
#include <termios.h>
#include <unistd.h>
#include <iomanip>

// Local
#include "uinput_helper.hpp"
#include "config_manager.hpp"
#include "window_monitor.hpp"

// 全局变量
int gUinputFileDescriptor = 0;
ConfigManager* gConfigManager = nullptr;
WindowMonitor* gWindowMonitor = nullptr;

void sigint_handler(sig_atomic_t /* s */)
{
	std::cout << "接收到中断信号，正在清理资源..." << std::endl;

	// 停止窗口监控
	if (gWindowMonitor) {
		gWindowMonitor->stop();
		delete gWindowMonitor;
		gWindowMonitor = nullptr;
	}

	// 清理配置管理器
	if (gConfigManager) {
		delete gConfigManager;
		gConfigManager = nullptr;
	}

	// 销毁虚拟输入设备
	destroyUinput(gUinputFileDescriptor);

	std::cout << "资源清理完成，退出程序" << std::endl;
	exit(0);
}

int main(int argc, char **argv)
{
	std::cout << "Tourbox Neo Linux 驱动程序启动" << std::endl;
	std::cout << "支持 Hyprland 窗口感知的动态配置" << std::endl;

	const int num_required_params = 2;

	if (argc != num_required_params)
	{
		std::cerr << "错误: 参数数量无效，期望 " << num_required_params << ", 实际接收到 " << argc << std::endl;
		std::cerr << "用法: " << argv[0] << " <串口设备路径>" << std::endl;
		return 1;
	}

	const std::string serialPortFile = argv[1];

	if (std::filesystem::exists(std::filesystem::path(serialPortFile)) == false)
	{
		std::cerr << "错误: 找不到串口设备文件 '" << serialPortFile << "'" << std::endl;
		return 1;
	}

	// 初始化配置管理器
	try {
		gConfigManager = new ConfigManager();
		std::cout << "配置管理器初始化成功" << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "配置管理器初始化失败: " << e.what() << std::endl;
		return 1;
	}

	// 初始化窗口监控器
	try {
		gWindowMonitor = new WindowMonitor();
		gWindowMonitor->start();
		std::cout << "窗口监控器启动成功" << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "窗口监控器启动失败: " << e.what() << std::endl;
		delete gConfigManager;
		return 1;
	}

	/// ---------- ///
	/// Setup and open a serial port ///

	const int serialPortFileDescriptor = open(serialPortFile.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);

	if (serialPortFileDescriptor == -1)
	{
		std::cerr << "Error: Failed to open serial port file " << serialPortFile << std::endl;
		return 1;
	}

	struct termios term_options;
	memset(&term_options, 0, sizeof(struct termios));

	term_options.c_cflag = B115200 | CS8 | CREAD;

	if ((tcsetattr(serialPortFileDescriptor, TCSANOW, &term_options)) != 0)
	{

		std::cerr << "Error: Failed to set termios settings";
		close(serialPortFileDescriptor);
		return 1;
	}

	if ((tcflush(serialPortFileDescriptor, TCIOFLUSH)) != 0)
	{

		std::cerr << "Error: Failed to flush termios settings";
		close(serialPortFileDescriptor);
		return 1;
	}

	// Wait for the serial port to open -- this might not be needed
	usleep(100000);

	std::array<uint8_t, 1> readBuffer;

	/// ---------- ///
	/// 设置虚拟输入设备

	// 获取所有需要注册的键码
	std::vector<int> allKeyCodes = gConfigManager->getAllKeyCodes();

	// 设置虚拟输入设备
	gUinputFileDescriptor = setupUinput(allKeyCodes);
	if (gUinputFileDescriptor < 0) {
		std::cerr << "设置虚拟输入设备失败" << std::endl;
		delete gWindowMonitor;
		delete gConfigManager;
		return 1;
	}

	std::cout << "虚拟输入设备设置成功" << std::endl;

	// 注册信号处理器，确保在程序终止时清理资源
	signal(SIGINT, sigint_handler);
	signal(SIGTERM, sigint_handler);

	// 等待虚拟设备初始化
	sleep(1);

	std::cout << "Tourbox Neo 驱动程序准备就绪，按 Ctrl+C 退出" << std::endl;

	while (true)
	{
		// 使用 select 来实现超时
		fd_set readfds;
		struct timeval timeout;
		
		// 设置超时为 100 毫秒
		timeout.tv_sec = 0;
		timeout.tv_usec = 100000;
		
		FD_ZERO(&readfds);
		FD_SET(serialPortFileDescriptor, &readfds);
		
		int selectResult = select(serialPortFileDescriptor + 1, &readfds, NULL, NULL, &timeout);
		
		if (selectResult == -1) {
			// 检查是否是因为信号中断
			if (errno == EINTR) {
				continue; // 被信号中断，重新循环
			}
			std::cerr << "select() 错误: " << strerror(errno) << std::endl;
			continue;
		}
		
		if (selectResult == 0) {
			// 超时，没有数据可读
			continue;
		}
		
		// 有数据可读
		ssize_t bytesRead = read(serialPortFileDescriptor, readBuffer.begin(), 1);

		if (bytesRead < 0)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// 非阻塞模式下暂时没有数据
				continue;
			}
			std::cerr << "从串口读取数据时出错: " << strerror(errno) << std::endl;
			continue; // 尝试继续运行而不是退出
		}

		if (bytesRead > 0)
		{
			// 获取当前窗口信息
			WindowInfo currentWindow = gWindowMonitor->getCurrentWindow();

			// 调试输出
			std::cout << std::hex << std::uppercase << std::setfill('0') << std::setw(2) 
				<< static_cast<int>(readBuffer[0]) << ": ";

			// 获取按键映射并生成事件
			int keyCode = gConfigManager->getKeyMapping(readBuffer[0], 
											   currentWindow.windowClass, 
											   currentWindow.windowTitle);

			// 如果没有映射，跳过
			if (keyCode == 0) {
				std::cout << "未映射的按钮代码: 0x" << std::hex << std::setfill('0') 
					<< std::setw(2) << static_cast<int>(readBuffer[0]) << std::endl;
				continue;
			}

			// 根据按钮代码输出按钮名称
			switch (readBuffer[0])
			{
				case 0x80:
					std::cout << "长键按下" << std::endl;
					break;
				case 0x81:
					std::cout << "侧键按下" << std::endl;
					break;
				case 0x82:
					std::cout << "横键按下" << std::endl;
					break;
				case 0x83:
					std::cout << "短键按下" << std::endl;
					break;
				case 0x4F:
					std::cout << "转盘顺时针" << std::endl;
					break;
				case 0x0F:
					std::cout << "转盘逆时针" << std::endl;
					break;
				case 0x90:
					std::cout << "D-Pad 上按下" << std::endl;
					break;
				case 0x91:
					std::cout << "D-Pad 下按下" << std::endl;
					break;
				case 0x92:
					std::cout << "D-Pad 左按下" << std::endl;
					break;
				case 0x93:
					std::cout << "D-Pad 右按下" << std::endl;
					break;
				case 0x8A:
					std::cout << "滚轮单击" << std::endl;
					break;
				case 0x49:
					std::cout << "滚轮上滚动" << std::endl;
					break;
				case 0x09:
					std::cout << "滚轮下滚动" << std::endl;
					break;
				case 0xAA:
					std::cout << "Tour 按钮按下" << std::endl;
					break;
				case 0xA2:
					std::cout << "C1 按钮按下" << std::endl;
					break;
				case 0xA3:
					std::cout << "C2 按钮按下" << std::endl;
					break;
				case 0x44:
					std::cout << "旋钮顺时针" << std::endl;
					break;
				case 0x04:
					std::cout << "旋钮逆时针" << std::endl;
					break;
				case 0xB7:
					std::cout << "旋钮单击" << std::endl;
					break;
				case 0xB8:
					std::cout << "转盘单击" << std::endl;
					break;
				default:
					std::cout << "未知按钮: 0x" << std::hex << std::setfill('0') 
						<< std::setw(2) << static_cast<int>(readBuffer[0]) << std::endl;
					break;
			}

			// 生成按键事件
			generateKeyPressEvent(gUinputFileDescriptor, keyCode);

			usleep(1000);
		}

		usleep(1000);
	}

	usleep(1000);

	// 清理资源
	if (gWindowMonitor) {
		gWindowMonitor->stop();
		delete gWindowMonitor;
	}

	if (gConfigManager) {
		delete gConfigManager;
	}

	destroyUinput(gUinputFileDescriptor);
	close(serialPortFileDescriptor);

	return 0;
}
