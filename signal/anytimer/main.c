#include "timer.h"
#include <stdio.h>
#include <stdlib.h>
void func1(char *a){
  printf("%s\n",a);
}
void func2(char *b){
  printf("%s\n",b);
}
int main(){
  initTimer();
  setTimer(3,func1,"aaa");
  setTimer(5,func2,"bbb");
  while(1);

  exit(0);
}
