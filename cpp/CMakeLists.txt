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

# 设置输出目录为项目根目录下的 build 目录
set_target_properties(tourbox_driver PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# 链接 nlohmann_json 库
target_link_libraries(tourbox_driver PRIVATE nlohmann_json::nlohmann_json)

# 添加调试标志（可选）
target_compile_options(tourbox_driver PRIVATE -g -O0 -Wall -Wextra -Wpedantic)