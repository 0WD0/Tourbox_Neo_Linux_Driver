/**
 * @file uinput_helper.hpp
 * @author Raleigh Littles <raleighlittles@gmail.com>
 * @brief Helper functions for working with Linux's `uinput` module.
 *        Based heavily on: https://www.kernel.org/doc/html/latest/input/uinput.html
 * @version 0.2
 * @date 2022-07-24
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef UINPUT_HELPER_HPP
#define UINPUT_HELPER_HPP

#include <vector>
#include <linux/uinput.h>

// 定义特殊映射常量 (用负值表示特殊操作，避免与标准键码冲突)
#define REL_X_POS (-1)  // 特殊值，表示鼠标右移
#define REL_X_NEG (-2)  // 特殊值，表示鼠标左移
#define REL_Y_POS (-3)  // 特殊值，表示鼠标下移
#define REL_Y_NEG (-4)  // 特殊值，表示鼠标上移

/**
 * @brief 发送输入事件
 * @param fileDescriptor 文件描述符
 * @param type 事件类型
 * @param code 事件代码
 * @param val 事件值
 */
void emit(int fileDescriptor, int type, int code, int val);

/**
 * @brief 根据键码生成按键事件
 * @param fileDescriptor 文件描述符
 * @param keyCode 键码
 */
void generateKeyPressEvent(int fileDescriptor, int keyCode);

/**
 * @brief 注册键盘事件
 * @param fileDescriptor 文件描述符
 * @param keyCodes 键码列表
 */
void registerKeyboardEvents(int fileDescriptor, const std::vector<int>& keyCodes);

/**
 * @brief 注册鼠标事件
 * @param fileDescriptor 文件描述符
 */
void registerMouseEvents(int fileDescriptor);

/**
 * @brief 设置虚拟输入设备
 * @param keyCodes 键码列表
 * @return 文件描述符
 */
int setupUinput(const std::vector<int>& keyCodes);

/**
 * @brief 销毁虚拟输入设备
 * @param fileDescriptor 文件描述符
 */
void destroyUinput(int fileDescriptor);

#endif // UINPUT_HELPER_HPP
