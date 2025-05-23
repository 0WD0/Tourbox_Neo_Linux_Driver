cmake_minimum_required(VERSION 3.10)

# 确保二进制文件输出到构建目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

project(TourboxNeoDriver VERSION 0.2)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加 cpp 子目录
add_subdirectory(cpp)

# 安装目标
install(TARGETS tourbox_driver DESTINATION bin)
