{
    files = {
        "main.c"
    },
    depfiles_gcc = "main.o: main.c timer.h\
",
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I.",
            "-g"
        }
    }
}