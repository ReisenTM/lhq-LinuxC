#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define LEFT 1010
#define RIGHT 1210
#define RANGE (RIGHT - LEFT + 1)
static pthread_t tid[RANGE];
static void *routine_GetPrime(void *p);
int main() {
  puts("begin!");

  for (int i = LEFT; i < RIGHT; i++) {
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
    pthread_join(tid[i - LEFT], NULL);
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
  free(p);
  pthread_exit(NULL);
}
