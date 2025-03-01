#include <pthread.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
static int64_t num;
void *add(void *p) {
  num++;
  pthread_exit(NULL);
}
int main(int argc, char *argv[]) {
  pthread_t tid;
  while (1) {

    int err = pthread_create(&tid, NULL, add, NULL);
    if (err) {
      fprintf(stdout, "max thread:%ld", num);
      exit(-1);
    }
  }
  return EXIT_SUCCESS;
}
