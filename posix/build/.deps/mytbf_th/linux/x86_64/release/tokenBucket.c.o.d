{
    depfiles_gcc = "tokenBucket.o: tokenBucket.c tokenBucket.h\
",
    values = {
        "/usr/bin/gcc",
        {
            "-m64",
            "-I."
        }
    },
    files = {
        "tokenBucket.c"
    }
}