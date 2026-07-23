cmake_minimum_required(VERSION 3.10)
project(scenic_map C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 导出 compile_commands.json，供 IDE（C/C++ 扩展 / clangd）做 IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# 关闭响应文件，让 -I 等参数直接内联到 compile_commands.json，便于 IDE 解析
set(CMAKE_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
set(CMAKE_USE_RESPONSE_FILE_FOR_OBJECTS OFF)

if(MSVC)
    # /utf-8 保证源码与执行字符集均为 UTF-8，中文显示正常
    add_compile_options(/W4 /utf-8)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra)
endif()

add_executable(scenic_map
    src/main.c
    src/data.c
    src/ui.c
    src/map.c
    src/graph.c
    src/route.c
)

target_include_directories(scenic_map PRIVATE src)

# 构建后把 data 目录拷贝到可执行文件所在目录，便于在任何位置运行
add_custom_command(TARGET scenic_map POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data
            $<TARGET_FILE_DIR:scenic_map>/data)
