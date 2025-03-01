-- 设置项目名称
set_project("my_timer")

-- 设置调试模式
--set_mode("debug")

-- 添加目标
target("timer")
-- 设置目标类型为可执行文件
set_kind("binary")

-- 添加源文件
add_files("main.c", "timer.c")

add_includedirs(".")

-- 添加调试信息
add_cflags("-g")
