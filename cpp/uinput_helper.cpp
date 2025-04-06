#include "uinput_helper.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

/**
 * @brief 发送输入事件
 * @param fileDescriptor 文件描述符
 * @param type 事件类型
 * @param code 事件代码
 * @param val 事件值
 */
void emit(int fileDescriptor, int type, int code, int val) {
    struct input_event event;
    memset(&event, 0, sizeof(event));
    
    event.type = type;
    event.code = code;
    event.value = val;
    
    // 设置时间戳
    gettimeofday(&event.time, NULL);
    
    // 写入事件
    if (write(fileDescriptor, &event, sizeof(event)) < 0) {
        std::cerr << "写入事件失败: " << strerror(errno) << std::endl;
    }
}

/**
 * @brief 根据键码生成按键事件
 * @param fileDescriptor 文件描述符
 * @param keyCode 键码
 */
void generateKeyPressEvent(int fileDescriptor, int keyCode) {
    // 处理特殊的鼠标移动映射
    if (keyCode == REL_X_POS) {
        // 鼠标右移
        emit(fileDescriptor, EV_REL, REL_X, 10);
        emit(fileDescriptor, EV_SYN, SYN_REPORT, 0);
        return;
    } else if (keyCode == REL_X_NEG) {
        // 鼠标左移
        emit(fileDescriptor, EV_REL, REL_X, -10);
        emit(fileDescriptor, EV_SYN, SYN_REPORT, 0);
        return;
    } else if (keyCode == REL_Y_POS) {
        // 鼠标下移
        emit(fileDescriptor, EV_REL, REL_Y, 10);
        emit(fileDescriptor, EV_SYN, SYN_REPORT, 0);
        return;
    } else if (keyCode == REL_Y_NEG) {
        // 鼠标上移
        emit(fileDescriptor, EV_REL, REL_Y, -10);
        emit(fileDescriptor, EV_SYN, SYN_REPORT, 0);
        return;
    }
    emit(fileDescriptor, EV_KEY, keyCode, 1);  // 按下
    emit(fileDescriptor, EV_SYN, SYN_REPORT, 0);
    usleep(10000);  // 10ms 延迟
    emit(fileDescriptor, EV_KEY, keyCode, 0);  // 释放
    emit(fileDescriptor, EV_SYN, SYN_REPORT, 0);
}

/**
 * @brief 注册键盘事件
 * @param fileDescriptor 文件描述符
 * @param keyCodes 键码列表
 */
void registerKeyboardEvents(int fileDescriptor, const std::vector<int>& keyCodes) {
    // 启用 EV_KEY 事件类型
    if (ioctl(fileDescriptor, UI_SET_EVBIT, EV_KEY) < 0) {
        std::cerr << "启用 EV_KEY 事件类型失败: " << strerror(errno) << std::endl;
    }

    // 注册所有键码
    for (int keyCode : keyCodes) {
        // 跳过特殊的鼠标移动映射
        if (keyCode == REL_X_POS || keyCode == REL_X_NEG || 
            keyCode == REL_Y_POS || keyCode == REL_Y_NEG) {
            continue;
        }

        // 注册键码
        if (ioctl(fileDescriptor, UI_SET_KEYBIT, keyCode) < 0) {
            std::cerr << "注册键码 " << keyCode << " 失败: " << strerror(errno) << std::endl;
        }
    }
}

/**
 * @brief 注册鼠标事件
 * @param fileDescriptor 文件描述符
 */
void registerMouseEvents(int fileDescriptor) {
    // 启用 EV_REL 事件类型（相对坐标）
    if (ioctl(fileDescriptor, UI_SET_EVBIT, EV_REL) < 0) {
        std::cerr << "启用 EV_REL 事件类型失败: " << strerror(errno) << std::endl;
    }

    // 启用 X 和 Y 轴移动
    if (ioctl(fileDescriptor, UI_SET_RELBIT, REL_X) < 0) {
        std::cerr << "启用 REL_X 失败: " << strerror(errno) << std::endl;
    }
    if (ioctl(fileDescriptor, UI_SET_RELBIT, REL_Y) < 0) {
        std::cerr << "启用 REL_Y 失败: " << strerror(errno) << std::endl;
    }

    // 启用鼠标滚轮
    if (ioctl(fileDescriptor, UI_SET_RELBIT, REL_WHEEL) < 0) {
        std::cerr << "启用 REL_WHEEL 失败: " << strerror(errno) << std::endl;
    }

    // 启用鼠标按钮
    if (ioctl(fileDescriptor, UI_SET_KEYBIT, BTN_LEFT) < 0) {
        std::cerr << "启用 BTN_LEFT 失败: " << strerror(errno) << std::endl;
    }
    if (ioctl(fileDescriptor, UI_SET_KEYBIT, BTN_RIGHT) < 0) {
        std::cerr << "启用 BTN_RIGHT 失败: " << strerror(errno) << std::endl;
    }
    if (ioctl(fileDescriptor, UI_SET_KEYBIT, BTN_MIDDLE) < 0) {
        std::cerr << "启用 BTN_MIDDLE 失败: " << strerror(errno) << std::endl;
    }
}

/**
 * @brief 设置虚拟输入设备
 * @param keyCodes 键码列表
 * @return 文件描述符
 */
int setupUinput(const std::vector<int>& keyCodes) {
    // 打开 uinput 设备
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        std::cerr << "打开 /dev/uinput 失败: " << strerror(errno) << std::endl;
        return -1;
    }

    // 启用同步事件
    if (ioctl(fd, UI_SET_EVBIT, EV_SYN) < 0) {
        std::cerr << "启用 EV_SYN 事件类型失败: " << strerror(errno) << std::endl;
        close(fd);
        return -1;
    }

    // 注册键盘事件
    registerKeyboardEvents(fd, keyCodes);

    // 注册鼠标事件
    registerMouseEvents(fd);

    // 设置设备信息
    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;   // 虚拟厂商 ID
    usetup.id.product = 0x5678;  // 虚拟产品 ID
    strcpy(usetup.name, "Tourbox Neo Virtual Input Device");

    // 创建设备
    if (ioctl(fd, UI_DEV_SETUP, &usetup) < 0) {
        std::cerr << "设置设备信息失败: " << strerror(errno) << std::endl;
        close(fd);
        return -1;
    }

    if (ioctl(fd, UI_DEV_CREATE) < 0) {
        std::cerr << "创建设备失败: " << strerror(errno) << std::endl;
        close(fd);
        return -1;
    }

    // 等待设备初始化
    sleep(1);

    return fd;
}

/**
 * @brief 销毁虚拟输入设备
 * @param fileDescriptor 文件描述符
 */
void destroyUinput(int fileDescriptor) {
    if (fileDescriptor <= 0) {
        return;
    }

    // 销毁设备
    if (ioctl(fileDescriptor, UI_DEV_DESTROY) < 0) {
        std::cerr << "销毁设备失败: " << strerror(errno) << std::endl;
    }

    // 关闭文件描述符
    close(fileDescriptor);
}
