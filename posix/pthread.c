#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void *routine(void *t) {
  puts("thread is working!");
  // return NULL;
  pthread_exit(NULL);
}
int main() {
  pthread_t tid;
  puts("begin");
  int err = pthread_create(&tid, NULL, routine, NULL);
  if (err) {
    fprintf(stderr, "err:%s\n", strerror(err));
  }
  puts("end");
  exit(0);
}
