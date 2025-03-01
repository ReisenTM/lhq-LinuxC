{
    files = {
        "main.c"
    },
    depfiles_gcc = "main.o: main.c tokenBucket.h\
",
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I."
        }
    }
}