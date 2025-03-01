#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define THRDNUM 3
#define LEFT 200
#define RIGHT 400
static pthread_mutex_t mut;
static pthread_t tid[THRDNUM];
typedef struct {
  int num;
  int flag;
} data;

static data n;
void *routine_GetPrime(void *p) { // 进程定义
  static int is_prime;
  while (1) {
    pthread_mutex_lock(&mut);
    // 如果有数据
    if (n.flag == 1)
      continue;
    printf("test:flag :%d and num: %d\n", n.flag, n.num);
    if (n.flag < 0)
      break;
    is_prime = 0;
    for (int i = 2; i < n.num / 2; i++) { // 求质数
      if (n.num % i == 0) {
        is_prime = -1;
        break;
      }
    }
    if (is_prime == 0) {
      printf("[thread %d]%d is prime\n", (int)p, n.num);
    }
    pthread_mutex_unlock(&mut);
  }
  pthread_exit(NULL);
}
int main(int argc, char *argv[]) {
  // initialize
  n.flag = 0;
  n.num = 0;
  // 默认值均为0，当flag==-1时数据全部传输完毕

  pthread_mutex_init(&mut, NULL);
  for (int i = 0; i < THRDNUM; i++) {
    pthread_create(&tid[i], NULL, routine_GetPrime, (void *)i); // 创建进程
  }
  for (int i = LEFT; i < RIGHT; i++) { // 求200-400间的质数
    pthread_mutex_lock(&mut);
    n.num = i;  // 放出数据
    n.flag = 1; // 标记位置1 ，表示有数据可以取
    while (n.flag != 0) {
      pthread_mutex_unlock(&mut);
      pthread_mutex_lock(&mut);
    }
    pthread_mutex_unlock(&mut);
    sched_yield(); // 让出调度器以便其他进程取走
    // n.flag = 0;
    // sleep(1);
  }
  pthread_mutex_lock(&mut);
  n.flag = -1; // 表示数据已经传完，进程可以凭此判断是否退出
  pthread_mutex_unlock(&mut);
  sched_yield();
  for (int i = 0; i < THRDNUM; i++) {
    pthread_join(tid[i], NULL); // 线程收尸
  }
  pthread_mutex_destroy(&mut);

  return EXIT_SUCCESS;
}
