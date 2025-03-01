{
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I.",
            "-g"
        }
    },
    depfiles_gcc = "timer.o: timer.c timer.h\
",
    files = {
        "timer.c"
    }
}