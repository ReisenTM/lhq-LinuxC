#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

void *mymalloc(void) {
  void *ret = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE,
                   MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (ret == NULL) {
    perror("mmap()");
    exit(EXIT_FAILURE);
  }
  return ret;
}
int main(int argc, char *argv[]) {

  int *p = NULL;
  p = mymalloc();
  *p = 4;
  fprintf(stdout, "p before delete:%d\n", *p);
  munmap(p, sizeof(int));
  return EXIT_SUCCESS;
}
