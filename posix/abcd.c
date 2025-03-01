#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define thrd_num 4
void *routine(void *p) {
  int out = 'a' + (int)p;
  while (1) {
    write(1, &out, 1);
  }
  pthread_exit(NULL);
}
int main(int argc, char *argv[]) {
  pthread_t tid[thrd_num];

  for (int i = 0; i < thrd_num; i++) {
    pthread_create(&tid[i], NULL, routine, (void *)i);
  }

  alarm(5);
  for (int i = 0; i < thrd_num; i++) {
    pthread_join(tid[i], NULL);
  }
  return EXIT_SUCCESS;
}
