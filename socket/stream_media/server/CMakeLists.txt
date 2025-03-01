cmake_minimum_required(VERSION 3.10)
project(ServerProject)

# 设置C标准
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 包含头文件目录
include_directories(
    ../include
    .
)

# 添加源文件
file(GLOB SERVER_SOURCES
    server.c
    medialib.c
    mytbf.c
    thr_channel.c
    thr_list.c
)

# 添加可执行文件
add_executable(server
    ${SERVER_SOURCES}
)

# 可选：设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
