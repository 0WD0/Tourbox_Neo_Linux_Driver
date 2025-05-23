#include <windows.h>
#include <stdio.h>
#include <string.h>

// 日志辅助函数
void log_call(const char* funcName, const char* format = "%s called\n", ...) {
    FILE* logFile = fopen("ble_wrapper_log.txt", "a");
    if (logFile) {
        fprintf(logFile, format, funcName);
        
        va_list args;
        va_start(args, format);
        vfprintf(logFile, format, args);
        va_end(args);
        
        fclose(logFile);
    }
}

// 根据 IDA Pro 分析导出的函数
extern "C" {
    // 1. getLocalBTAddr - 获取本地蓝牙地址
    __declspec(dllexport) const char* getLocalBTAddr() {
        log_call("getLocalBTAddr");
        static char addr[] = "00:11:22:33:44:55"; // 模拟的蓝牙地址
        return addr;
    }
    
    // 2. getLocalBTState - 获取本地蓝牙状态
    __declspec(dllexport) int getLocalBTState() {
        log_call("getLocalBTState");
        return 2; // 假设 2 代表蓝牙已开启状态
    }
    
    // 3. setBTNotify - 设置蓝牙通知回调（这是导致崩溃的函数）
    __declspec(dllexport) int setBTNotify(void* callback) {
        log_call("setBTNotify", "%s called with callback: %p\n", "setBTNotify", callback);
        return 0; // 成功
    }
    
    // 4. setCallbackByte - 设置回调字节
    __declspec(dllexport) int setCallbackByte(void* callback) {
        log_call("setCallbackByte", "%s called with callback: %p\n", "setCallbackByte", callback);
        return 0; // 成功
    }
    
    // 5. init - 初始化
    __declspec(dllexport) int init() {
        log_call("init");
        return 0; // 成功
    }
    
    // 6. setNamePrefix - 设置名称前缀
    __declspec(dllexport) int setNamePrefix(const char* prefix) {
        log_call("setNamePrefix", "%s called with prefix: %s\n", "setNamePrefix", prefix ? prefix : "NULL");
        return 0; // 成功
    }
    
    // 7. scan - 扫描设备
    __declspec(dllexport) int scan() {
        log_call("scan");
        return 1; // 返回找到的设备数量
    }
    
    // 8. start - 开始连接
    __declspec(dllexport) int start() {
        log_call("start");
        return 0; // 成功
    }
    
    // 9. stop - 停止连接
    __declspec(dllexport) int stop() {
        log_call("stop");
        return 0; // 成功
    }
    
    // 10. writeValue - 写入数据
    __declspec(dllexport) int writeValue(const char* data, int length) {
        log_call("writeValue", "%s called with data length: %d\n", "writeValue", length);
        return length; // 返回写入的长度
    }
    
    // 11. setValidDeviceAddrList - 设置有效设备地址列表
    __declspec(dllexport) int setValidDeviceAddrList(const char** addrList, int count) {
        log_call("setValidDeviceAddrList", "%s called with count: %d\n", "setValidDeviceAddrList", count);
        return 0; // 成功
    }
    
    // 12. setPairMode - 设置配对模式
    __declspec(dllexport) int setPairMode(int mode) {
        log_call("setPairMode", "%s called with mode: %d\n", "setPairMode", mode);
        return 0; // 成功
    }
    
    // 13. nativeExit - 退出
    __declspec(dllexport) int nativeExit() {
        log_call("nativeExit");
        return 0; // 成功
    }
}

// DLL entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            // Initialize the DLL
            {
                FILE* logFile = fopen("ble_wrapper_log.txt", "w");
                if (logFile) {
                    fprintf(logFile, "Mock libBLEWrapper.dll loaded\n");
                    fclose(logFile);
                }
            }
            break;
            
        case DLL_PROCESS_DETACH:
            // Clean up the DLL
            {
                FILE* logFile = fopen("ble_wrapper_log.txt", "a");
                if (logFile) {
                    fprintf(logFile, "Mock libBLEWrapper.dll unloaded\n");
                    fclose(logFile);
                }
            }
            break;
    }
    
    return TRUE;
}
