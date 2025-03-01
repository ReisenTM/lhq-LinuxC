#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define FILENAME "/tmp/out"
#define BUFSIZE 1024
static char strbuf[BUFSIZE];
void *routine(void *p) {
  FILE *fp = fopen(FILENAME, "r+");
  if (fp == NULL) {
    perror("fopen");
    exit(-1);
  }
  fgets(strbuf, BUFSIZE, fp);
  fseek(fp, 0, SEEK_SET);
  fprintf(fp, "%d\n", atoi(strbuf) + 1);
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
  return EXIT_SUCCESS;
}
