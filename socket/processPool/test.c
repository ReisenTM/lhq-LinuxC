#include "proto.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define lowestIdleProcess 5
#define maxIdleProcess 10
#define maxWorkingProcess 20
#define MYSIG SIGUSR2
#define STRSIZE 40
// 状态
enum { STATE_IDLE = 0, STATE_BUSY };

// 任务
struct task_t {
  pid_t pid;
  int state;
};

// 进程池
static struct task_t *pool;
static int idel_count, busy_count;
static int sockfd;
static time_t tim;
static char strbuf[STRSIZE];
void Signal_Handler(int s) { return; }
static void server_job(int pos) {
  int newfd;
  struct sockaddr_in raddr;
  int len = 0;
  time_t tim;
  socklen_t size = sizeof(raddr);
  pid_t ppid = getppid();
  while (1) {
    pool[pos].state = STATE_IDLE;
    kill(ppid, MYSIG);
    newfd = accept(sockfd, (void *)&raddr, &size);
    if (newfd < 0) {
      if (newfd != EINTR || newfd != EAGAIN) {
        perror("accept");
        exit(EXIT_FAILURE);
      }
    }
    pool[pos].state = STATE_BUSY;
    kill(ppid, MYSIG); // 通知父进程去看
    tim = time(NULL);
    len = snprintf(strbuf, STRSIZE, "%lld\n", (long long)tim);
    send(newfd, strbuf, len, 0);
    close(newfd);
    sleep(5);
  }
}
static int add_single_task() {
  int slot;
  pid_t pid;
  if (idel_count + busy_count > maxWorkingProcess)
    return -1;
  for (slot = 0; slot < maxWorkingProcess; slot++) {
    if (pool[slot].pid == -1) {
      break;
    }
  }
  pool[slot].state = STATE_IDLE;
  pid = fork();
  if (pid < 0) {
    perror("fork()");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    server_job(slot);
    exit(0);
  } else {
    idel_count++;
    pool[slot].pid = pid;
  }
  return 0;
}

static int del_single_task() {
  if (idel_count == 0)
    return -1;
  for (int i = 0; i < maxWorkingProcess; i++) {
    if (pool[i].pid != -1 && pool[i].state == STATE_IDLE) {
      kill(pool[i].pid, SIGTERM);
      pool[i].pid = -1;
      idel_count--;
      break;
    }
  }
  return 0;
}

static void scan_pool() {
  int idle = 0, busy = 0;
  for (int i = 0; i < maxWorkingProcess; i++) {
    if (pool[i].pid == -1) {
      continue;
    } else if (kill(pool[i].pid, 0)) {
      pool[i].pid = -1;
      continue;
    }
    if (pool[i].state == STATE_IDLE)
      idle++;
    else if (pool[i].state == STATE_BUSY) {
      busy++;
    } else {
      printf("wtf?!\n");
      abort();
    }
  }
  idel_count = idle;
  busy_count = busy;
}
static void scan_pool2() {
  int idle = 0, busy = 0;
  for (int i = 0; i < maxWorkingProcess; i++) {
    if (pool[i].pid == -1) {
      continue;
    }
    if (kill(pool[i].pid, 0)) { // kill pid 0检测一个进程是否存在
      pool[i].pid = -1;
      continue;
    }
    // 统计进程池的状态
    if (pool[i].state == STATE_IDLE)
      idle++;
    else if (pool[i].state == STATE_BUSY)
      busy++;
    else {
      fprintf(stderr, "未知状态!\n");
      abort();
    }
  }
  idel_count = idle;
  busy_count = busy;
}
static void print_pool() {
  for (int i = 0; i < maxWorkingProcess; i++) {
    if (pool[i].pid == -1)
      putchar('_');
    else if (pool[i].state == STATE_IDLE)
      putchar('*');
    else {
      putchar('x');
    }
    fflush(NULL);
  }
  printf("\n");
}
int main(int argc, char *argv[]) {
  // get socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // set addr reuse
  int val = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  // bind addr to socket
  struct sockaddr_in localaddr;
  memset(&localaddr, 0, sizeof(localaddr));
  localaddr.sin_port = htons(atoi(SERVERPORT));
  localaddr.sin_family = AF_INET;
  inet_pton(AF_INET, "0.0.0.0", &localaddr.sin_addr);

  if (bind(sockfd, (void *)&localaddr, sizeof(localaddr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  // listen to port
  if (listen(sockfd, 100) < 0) {
    perror("listne");
    exit(EXIT_FAILURE);
  }

  // child process self clean
  struct sigaction sa, osa;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = SIG_IGN;
  sa.sa_flags = SA_NOCLDWAIT;
  sigaction(SIGCHLD, &sa, &osa);

  // seft designed signal
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = Signal_Handler;
  sa.sa_flags = 0;
  sigaction(MYSIG, &sa, &osa);
  // set MYSIG blocked
  sigset_t set, oset;
  sigaddset(&set, MYSIG);
  if (sigprocmask(SIG_BLOCK, &set, &oset) < 0) {
    perror("sigprocmask");
    exit(EXIT_FAILURE);
  }
  // malloc memory for pool
  pool = mmap(NULL, sizeof(struct task_t) * maxWorkingProcess,
              PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
  if (pool == NULL) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  // initialize: init pool
  for (int i = 0; i < maxWorkingProcess; i++) {
    pool[i].pid = -1;
  }

  // initialize:fork 4 process
  for (int i = 0; i < lowestIdleProcess; i++) {
    add_single_task();
  }

  while (1) {
    sigsuspend(&oset);
    scan_pool2();

    // control pool
    // if idel task pass the limit ,delete them
    if (idel_count > maxIdleProcess) {
      for (int i = 0; i < (idel_count - maxIdleProcess); i++) {
        del_single_task();
      }
    } else if (idel_count < lowestIdleProcess) {
      for (int i = 0; i < (lowestIdleProcess - idel_count); i++) {
        add_single_task();
      }
    }
    print_pool();
  }

  // 恢复
  close(sockfd);
  sigprocmask(SIG_SETMASK, &oset, NULL);
  munmap(pool, sizeof(*pool) * maxWorkingProcess);
  sigaction(MYSIG, &osa, &sa);
  return EXIT_SUCCESS;
}
