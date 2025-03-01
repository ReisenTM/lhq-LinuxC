# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 设置项目名称和版本
project(anytimer VERSION 1.0)


# 添加可执行文件
add_executable(anytimer
    main.c
    timer.c
    timer.h
)

# 设置编译选项（可选）
# 例如，启用调试信息
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -g")

