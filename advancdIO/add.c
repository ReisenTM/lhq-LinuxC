#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <unistd.h>
#define FILENAME "/tmp/out"
#define BUFSIZE 1024
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
static char strbuf[BUFSIZE];
void *routine(void *p) {
  FILE *fp = fopen(FILENAME, "r+");
  if (fp == NULL) {
    perror("fopen");
    exit(-1);
  }
  pthread_mutex_lock(&mut);
  int fd = fileno(fp);
  lockf(fd, F_LOCK, 0);
  fgets(strbuf, BUFSIZE, fp);
  fseek(fp, 0, SEEK_SET);
  // sleep(1);
  fprintf(fp, "%d\n", atoi(strbuf) + 1);
  fflush(fp);
  lockf(fd, F_ULOCK, 0);
  pthread_mutex_unlock(&mut);
  fclose(fp);
  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
  pthread_t tid[200];
  for (int i = 0; i < 200; i++) {
    pthread_create(&tid[i], NULL, routine, NULL);
  }
  for (int i = 0; i < 200; i++) {
    pthread_join(tid[i], NULL);
  }
  pthread_mutex_destroy(&mut);
  return EXIT_SUCCESS;
}
