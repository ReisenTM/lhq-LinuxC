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
static struct fsm_st fsm_s;
static struct msg_st mst;
static int fd;
static int msqid;

static int server_Init() {
  mst.flag = 1;
  fsm_s.state = S_WAIT;
  key_t key = ftok(KEYPATH, KEYNUM);
  int ret = msgget(key, IPC_CREAT | 0600); // 创建msq，权限600
  if (ret < 0) {
    perror("msgget()");
    exit(EXIT_FAILURE);
  }
  puts("msq created!");
  return ret;
}
static int ret;
static int pos;
static void fsm() {
  switch (fsm_s.state) {
  case S_READ:
    puts("reading!");
    if (strcmp(mst.filename, "end") == 0) {
      puts("end!");
      mst.flag = ALLEND;
      break;
    }
    fd = open(mst.filename, O_RDONLY);
    if (fd < 0) {
      perror("OPEN");
      exit(EXIT_FAILURE);
    }
    if (read(fd, mst.data, MAXSIZE) < 0) {
      perror("read");
      close(fd);
      exit(EXIT_FAILURE);
    }
    fsm_s.state = S_WRITE;
    break;
  case S_WRITE:
    puts("writing!");
    mst.msgtyp = 1;
    if (msgsnd(msqid, &mst, sizeof(mst) - sizeof(long), 0) < 0) {
      perror("msgsnd");
      exit(EXIT_FAILURE);
    }
    close(fd);
    fsm_s.state = S_DONE;
    break;
  default:
    puts("default");
    break;
  }
}
int main(int argc, char *argv[]) {
  msqid = server_Init();
  while (mst.flag != ALLEND) { // 如果flag不为END
    if (msgrcv(msqid, &mst, sizeof(mst) - sizeof(long), 0, 0) < 0) {
      perror("msgrcv() -74");

      printf("sizeof mst size:%ld\n", sizeof(mst));
      exit(EXIT_FAILURE);
    }

    fsm_s.state = S_READ;
    while (fsm_s.state != S_DONE) { // 若未传输结束，不停推状态机
      fsm();
    }
    fsm_s.state = S_WAIT;
  }
  msgctl(msqid, IPC_RMID, 0);
  return EXIT_SUCCESS;
}
