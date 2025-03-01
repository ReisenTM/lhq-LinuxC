# 指定所需的最低CMake版本
cmake_minimum_required(VERSION 3.10)

# 设置项目名称及其版本号
project(dgram VERSION 1.0)

# 查找标准C库（通常不需要手动指定，这里仅作示例）
# find_package(StandardC REQUIRED)

# 添加包含目录，如果头文件不在当前目录下，则需要指定路径
include_directories(${PROJECT_SOURCE_DIR})

# 添加可执行文件，第一个参数是输出的可执行文件名，
# 后续参数是要编译的源文件列表
add_executable(dgram 
    receiver.c
    sender.c
)
