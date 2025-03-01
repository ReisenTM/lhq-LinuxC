#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define THRDNUM 3
#define LEFT 200
#define RIGHT 400

static pthread_mutex_t mut;
static pthread_cond_t cond; // 添加条件变量
static pthread_t tid[THRDNUM];

typedef struct {
  int num;
  int flag;
} data;

static data n;

void *routine_GetPrime(void *p) {
  while (1) {
    pthread_mutex_lock(&mut);

    // 如果没有数据，等待条件变量
    while (n.flag != 1) {
      if (n.flag == -1) {
        pthread_mutex_unlock(&mut);
        pthread_exit(NULL); // 退出线程
      }
      pthread_cond_wait(&cond, &mut); // 等待条件变量
    }

    // 获取数据
    int num = n.num;
    n.flag = 0;                 // 重置标志位
    pthread_cond_signal(&cond); // 通知主线程可以放入新数据
    pthread_mutex_unlock(&mut);

    // 计算质数
    int is_prime = 1;
    for (int i = 2; i < num / 2; i++) {
      if (num % i == 0) {
        is_prime = 0;
        break;
      }
    }

    if (is_prime) {
      printf("[thread %d] %d is prime\n", (int)(long)p, num);
    }
  }
}

int main(int argc, char *argv[]) {
  // 初始化
  n.flag = 0;
  n.num = 0;
  pthread_mutex_init(&mut, NULL);
  pthread_cond_init(&cond, NULL);

  // 创建线程
  for (int i = 0; i < THRDNUM; i++) {
    pthread_create(&tid[i], NULL, routine_GetPrime, (void *)(long)i);
  }

  // 分配任务
  for (int i = LEFT; i < RIGHT; i++) {
    pthread_mutex_lock(&mut);

    // 等待线程取走数据
    while (n.flag != 0) {
      pthread_cond_wait(&cond, &mut);
    }

    // 放入新数据
    n.num = i;
    n.flag = 1;
    pthread_cond_signal(&cond); // 通知线程有新数据
    pthread_mutex_unlock(&mut);
  }

  // 等待所有任务完成
  pthread_mutex_lock(&mut);
  while (n.flag != 0) {
    pthread_cond_wait(&cond, &mut);
  }
  n.flag = -1;                   // 标记任务结束
  pthread_cond_broadcast(&cond); // 唤醒所有线程
  pthread_mutex_unlock(&mut);

  // 等待线程退出
  for (int i = 0; i < THRDNUM; i++) {
    pthread_join(tid[i], NULL);
  }

  // 销毁资源
  pthread_mutex_destroy(&mut);
  pthread_cond_destroy(&cond);

  return EXIT_SUCCESS;
}
