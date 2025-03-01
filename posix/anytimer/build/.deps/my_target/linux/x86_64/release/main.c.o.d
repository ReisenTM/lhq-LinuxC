{
    files = {
        "main.c"
    },
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-g"
        }
    },
    depfiles_gcc = "main.o: main.c timer.h\
"
}