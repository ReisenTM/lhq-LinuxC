#include "sem.h"
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define LEFT 1010
#define RIGHT 1210
#define RANGE (RIGHT - LEFT + 1)
#define THRDNUM 4
static pthread_t tid[RANGE];
static void *routine_GetPrime(void *p);
static struct sem_t *sem;
int main() {
  static void *p;
  puts("begin!");
  sem = Sem_Init(THRDNUM); // 信号量初始化
  if (sem == NULL) {
    exit(1);
    fprintf(stderr, "malloc\n");
  }
  for (int i = LEFT; i < RIGHT; i++) {
    Sem_Sub(sem, 1); // 如果信号量允许，创建线程,否则等待
    int *tmp = malloc(sizeof(int));
    *tmp = i;
    int err =
        pthread_create(&tid[i - LEFT], NULL, routine_GetPrime, tmp); // 生成线程
    if (err) { // 如果失败则报错退出
      fprintf(stderr, "%s\n", strerror(err));
      exit(-1);
    }
  }

  for (int i = LEFT; i < RIGHT; i++) { // 进程收尸
    pthread_join(tid[i - LEFT], p);    // p接收线程返回值
    free(p); // 因为只有收到尸才会继续程序，可以实现排队释放内存
  }
  puts("end!");
  exit(0);
}

static void *routine_GetPrime(void *p) {
  if (p == NULL) {
    fprintf(stderr, "error!\n");
    return NULL;
  }
  int n = *((int *)p);
  int8_t flag = 1;
  for (int i = 2; i < n / 2; i++) {
    if (n % i == 0) {
      flag = 0;
      break;
    }
  }
  if (flag)
    printf("%d is a prime\n", n);
  // free(p);
  Sem_Ret(sem, 1); // 线程结束，归还信号量
  pthread_exit(p);
}
