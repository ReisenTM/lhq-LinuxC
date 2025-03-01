#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void handler(int signum, siginfo_t *info, void *context) {
    printf("Received signal: %d\n", signum);
    printf("Sender PID: %d\n", info->si_pid);
    printf("Sender UID: %d\n", info->si_uid);
}

int main() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = handler;          // 使用复杂的信号处理函数
    sigemptyset(&sa.sa_mask);           // 清空信号屏蔽字
    sa.sa_flags = SA_SIGINFO;           // 使用 sa_sigaction

    // 设置 SIGINT 的处理函数
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("Press Ctrl+C to send SIGINT...\n");
    while (1) {
        sleep(1);
    }

    return 0;
}
