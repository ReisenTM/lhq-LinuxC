#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define thrd_num 4
static int status;
static pthread_mutex_t mut;
void *routine(void *p) {
  int serial = (int)p;
  int out = 'a' + serial;

  while (1) {
    pthread_mutex_lock(&mut);
    status %= 4;
    if (status == serial) {
      write(1, &out, 1);
      status++;
    }
    pthread_mutex_unlock(&mut);
  }
  pthread_exit(NULL);
}
int main(int argc, char *argv[]) {
  pthread_t tid[thrd_num];
  pthread_mutex_init(&mut, NULL); // init

  for (int i = 0; i < thrd_num; i++) {
    pthread_create(&tid[i], NULL, routine, (void *)i);
  }

  alarm(5);
  for (int i = 0; i < thrd_num; i++) {
    pthread_join(tid[i], NULL);
  }
  pthread_mutex_destroy(&mut); // destroy
  return EXIT_SUCCESS;
}
