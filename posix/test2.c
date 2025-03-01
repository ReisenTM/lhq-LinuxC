#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void *routine(void *t) {
  puts("thread is working!");
  // return NULL;
  char *str = "test";
  pthread_exit(str);
}
int main() {
  pthread_t tid;
  puts("begin");
  int err = pthread_create(&tid, NULL, routine, NULL);
  if (err) {
    fprintf(stderr, "err:%s\n", strerror(err));
  }

  char *s;
  pthread_join(tid, &s);
  puts(s);
  puts("end");

  exit(0);
}
