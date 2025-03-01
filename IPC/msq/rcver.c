#include "proto.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
static int msgid;
struct sigaction sa_dft;
struct sigaction *sa;
void Sig_Handler(int signum) {      // 信号处理函数
  msgctl(msgid, IPC_RMID, 0);       // 删除message queue
  sigaction(SIGINT, &sa_dft, NULL); // 恢复默认设置
  free(sa);
}
static void Sig_Init() {
  sa = malloc(sizeof(*sa));
  sa->sa_flags = 0;          // 无特殊标识
  sigemptyset(&sa->sa_mask); // 清空信号掩码
  sa->sa_handler = Sig_Handler;
  if (sigaction(SIGINT, sa, &sa_dft) < 0) { // 替换INT信号为指定函数
    perror("sigaction()");
    exit(EXIT_FAILURE);
  }
}
int main(int argc, char *argv[]) {
  Sig_Init();
  static struct msgst mst;
  key_t key = ftok(KEYPATH, KEYID);
  msgid = msgget(key, IPC_CREAT | 0600);
  while (1) {
    if (msgrcv(msgid, &mst, sizeof(mst) - sizeof(long), 0, 0) < 0) {
      perror("msgrcv()");
      exit(EXIT_FAILURE);
    }
    printf("NAME:%s\n", mst.name);
    printf("SCORE:%d\n", mst.score);
  }
  return EXIT_SUCCESS;
}
