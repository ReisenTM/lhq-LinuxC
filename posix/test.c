#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int8_t flag;
void *routine(void *t) {
  puts("thread is working!");
  // return NULL;
  flag = 1;
  pthread_exit(NULL);
}
int main() {
  pthread_t tid;
  puts("begin");
  int err = pthread_create(&tid, NULL, routine, NULL);
  if (err) {
    fprintf(stderr, "err:%s\n", strerror(err));
  }
  while (!flag)
    ;
  puts("end");
  exit(0);
}
