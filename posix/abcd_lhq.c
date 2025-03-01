#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define thrd_num 4
static pthread_mutex_t mut[thrd_num];
static int next(int n) {
  if (n + 1 == 4)
    return 0;
  else
    return n + 1;
}
void *routine(void *p) {
  int serial = (int)p;
  int out = 'a' + serial;
  while (1) {
    pthread_mutex_lock(&mut[serial]);
    write(1, &out, 1);
    // pthread_mutex_unlock(&mut[next(serial)]);
    pthread_mutex_unlock(&mut[((serial + 1) % 4)]);
  }
  pthread_exit(NULL);
}
int main(int argc, char *argv[]) {
  pthread_t tid[thrd_num];

  for (int i = 0; i < thrd_num; i++) {
    pthread_mutex_init(&mut[i], NULL);
    pthread_mutex_lock(&mut[i]); //?
    pthread_create(&tid[i], NULL, routine, (void *)i);
  }
  pthread_mutex_unlock(&mut[0]); // unlock a  for lauch
  alarm(5);
  for (int i = 0; i < thrd_num; i++) {
    pthread_join(tid[i], NULL);
  }
  return EXIT_SUCCESS;
}
