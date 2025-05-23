# 设置源文件
set(SOURCES
    main.cpp
    uinput_helper.cpp
    config_manager.cpp
    window_monitor.cpp
)

# 设置头文件
set(HEADERS
    uinput_helper.hpp
    config_manager.hpp
    window_monitor.hpp
)

# 创建可执行文件
add_executable(tourbox_driver ${SOURCES} ${HEADERS})

# 查找 nlohmann_json 库
find_package(nlohmann_json REQUIRED)

# 链接 nlohmann_json 库
target_link_libraries(tourbox_driver PRIVATE nlohmann_json::nlohmann_json)

# 添加调试标志（可选）
target_compile_options(tourbox_driver PRIVATE -g -O0 -Wall -Wextra -Wpedantic)
