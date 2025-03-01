#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define BUFSIZE 1024

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    int fds, fdd = 1;

    // 反复尝试打开文件，直到成功或发生非 EINTR 错误
    do {
        fds = open(argv[1], O_RDONLY);
        if (fds < 0) {
            if (errno != EINTR) {
                perror("open");
                exit(1);
            }
        }
    } while (fds < 0);

    int pos = 0, res, len;
    char buf[BUFSIZE];

    // 读取文件内容并写入标准输出
    while (1) {
        len = read(fds, buf, BUFSIZE);
        if (len < 0) {
            if (errno == EINTR)
                continue;
            perror("read");
            break;
        }
        if (len == 0)
            break;

        pos = 0;
        while (len > 0) {
            res = write(fdd, buf + pos, len);
            if (res < 0) {
                if (errno == EINTR)
                    continue;
                perror("write");
                exit(1);
            }
            pos += res;
            len -= res;
        }
    }

    close(fds);
    exit(0);
}
