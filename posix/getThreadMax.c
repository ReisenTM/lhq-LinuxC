#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 线程函数，什么也不做
void *thread_function(void *arg) {
  while (1) {
    sleep(1); // 保持线程存活
  }
  return NULL;
}

int main() {
  int thread_count = 0;
  pthread_t thread;

  while (1) {
    // 创建线程
    if (pthread_create(&thread, NULL, thread_function, NULL) != 0) {
      perror("pthread_create failed");
      break;
    }
    thread_count++;
    printf("Created thread %d\n", thread_count);
  }

  printf("Maximum number of threads reached: %d\n", thread_count);
  return 0;
}
