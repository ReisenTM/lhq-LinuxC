#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void fun1(char *a) { puts(a); }
void fun2(char *b) { puts(b); }
int main(int argc, char *argv[]) {
  initTimer();
  setTimer(3, fun1, "sss");
  setTimer(5, fun2, "bbbb");
  while (1) {
    sleep(1);
  }
  delTimer();
  return EXIT_SUCCESS;
}
