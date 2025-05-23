# Makefile for mock libBLEWrapper.dll

# === Windows 环境下使用 ===

# Visual Studio 命令行编译器 (cl.exe)
WIN_CXX = cl.exe
WIN_CXXFLAGS = /LD /EHsc /O2 /D_WINDLL /D_UNICODE /DUNICODE

# MinGW 编译器
MINGW_CXX = g++
MINGW_CXXFLAGS = -Wall -shared -std=c++11 -o

# 目标 DLL
TARGET = libBLEWrapper.dll

# 源文件
SOURCES = libBLEWrapper.cpp

# 默认目标 - 检测编译器
all:
	@echo "\n请选择使用的编译器："
	@echo "  make cl - 使用 Visual Studio 编译器 (cl.exe)"
	@echo "  make mingw - 使用 MinGW 编译器 (g++)"

# 使用 Visual Studio 编译器
cl:
	$(WIN_CXX) $(WIN_CXXFLAGS) $(SOURCES) /Fe:$(TARGET)

# 使用 MinGW 编译器
mingw:
	$(MINGW_CXX) $(MINGW_CXXFLAGS) $(TARGET) $(SOURCES)

# 清理
clean:
	if exist $(TARGET) del $(TARGET)

# 安装 - 复制到 TourBox Console 目录
install:
	@echo "Installing $(TARGET) to TourBox Console directory..."
	@if not exist "C:\Program Files\TourBox Console\win64libs\libBLEWrapper.dll.bak" (
		copy "C:\Program Files\TourBox Console\win64libs\libBLEWrapper.dll" "C:\Program Files\TourBox Console\win64libs\libBLEWrapper.dll.bak"
	)
	copy $(TARGET) "C:\Program Files\TourBox Console\win64libs\"
	@echo "Done!"

# === Linux 环境下使用 (Wine) ===

# 在 Linux 下使用 MinGW 交叉编译
LINUX_CXX = x86_64-w64-mingw32-g++
LINUX_CXXFLAGS = -Wall -shared -std=c++11

# Linux 下编译
linux:
	$(LINUX_CXX) $(LINUX_CXXFLAGS) -o $(TARGET) $(SOURCES)

# Linux 下清理
linux-clean:
	rm -f $(TARGET)

# Linux 下安装
linux-install:
	@echo "Installing $(TARGET) to Wine TourBox Console directory..."
	if [ -f "$(HOME)/.wine/drive_c/Program Files/TourBox Console/win64libs/libBLEWrapper.dll" ]; then \
		cp "$(HOME)/.wine/drive_c/Program Files/TourBox Console/win64libs/libBLEWrapper.dll" "$(HOME)/.wine/drive_c/Program Files/TourBox Console/win64libs/libBLEWrapper.dll.bak"; \
	fi
	cp $(TARGET) "$(HOME)/.wine/drive_c/Program Files/TourBox Console/win64libs/"
	@echo "Done!"

.PHONY: all cl mingw clean install linux linux-clean linux-install
