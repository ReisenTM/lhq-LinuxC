#include "proto.h"
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
struct fsm_st {
  int8_t state;
  char *errmsg;
};
enum { S_WAIT = 0, S_READ, S_WRITE, S_DONE };

static struct msg_st mst;
static struct fsm_st fsm_c;
static int msqid;

static int client_Init() {
  mst.flag = 1;
  fsm_c.state = S_WAIT;
  key_t key = ftok(KEYPATH, KEYNUM);
  return msgget(key, 0);
}

static void fsm() {
  switch (fsm_c.state) {
  case S_READ:
    if (msgrcv(msqid, &mst, sizeof(mst) - sizeof(long), 0, 0) < 0) {
      perror("msgrcv");
      exit(EXIT_FAILURE);
    }
    fsm_c.state = S_WRITE;
    break;
  case S_WRITE:
    fprintf(stdout, "%s\n", mst.data);
    // fsm_c.state = S_READ;

    fsm_c.state = S_DONE;
    break;
  default:
    puts("default");
    break;
  }
}
static void string_Handler(char *s) {
  int pos = strcspn(s, "\n");
  s[pos] = '\0';
}
int main(int argc, char *argv[]) {
  msqid = client_Init();
  while (mst.flag != ALLEND) {
    printf("请输入要查看的文件名:\n");
    // scanf("%s", mst.filename);
    if (read(0, mst.filename, NAMESIZE) < 0) {
      perror("read");
      exit(EXIT_FAILURE);
    }
    string_Handler(mst.filename);
    if (strcmp(mst.filename, "end") == 0) { // 如果为end直接退出
      fputs("end!!", stdout);
      break;
    }
    mst.msgtyp = 1;
    if (msgsnd(msqid, &mst, sizeof(mst) - sizeof(long), 0) < 0) {
      perror("msgsnd()");
      exit(EXIT_FAILURE);
    }
    fsm_c.state = S_READ;
    while (fsm_c.state != S_DONE) {
      fsm();
    }

    fsm_c.state = S_WAIT;
  }
  msgctl(msqid, IPC_RMID, 0);
  return EXIT_SUCCESS;
}
