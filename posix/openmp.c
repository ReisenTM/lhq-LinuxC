#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
#pragma omp parallel
  {
    puts("hello");
    puts("world");
  }
  return EXIT_SUCCESS;
}
