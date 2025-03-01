#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include "timer.h"
#define MAXSIZE 1024
typedef void TimHandler_t(void *);
typedef struct timer{
  TimHandler_t* Tim_Handler;
  int sec;
  void *arg;
}timer;
static timer* pool[MAXSIZE];
typedef void (*sighandler_t)(int);

static void delTimer(int number){
  timer* tmp=pool[number];
  pool[number]=NULL;
  free(tmp);
}

static void checkTimer(){
  for(int i =0;i<MAXSIZE;i++){
    if(pool[i]!=NULL){
      if((pool[i]->sec)>0)
        pool[i]->sec--;
      if(pool[i]->sec==0){
        pool[i]->Tim_Handler(pool[i]->arg);
        delTimer(i);
      }
    }
  }
}
static void alrm_handler(int s){
  checkTimer();
  alarm(1);
}
void initTimer(){
  signal(SIGALRM,alrm_handler);
  alarm(1);
}

//输入示例 setTimer(3,f1,"aaa")
/*
 *parameter:none
 *retval: success or not
 *def:find a position for a timer 
 * */

static int t_findPos(){
  for(int i=0;i<MAXSIZE;i++){
    if(pool[i]==NULL) return i;
  }
  return -1;
}


int setTimer(int sec,TimHandler_t* func,void *arg){
  int pos=t_findPos(); 
  if(pos<0) return -1;
  timer* tm=malloc(sizeof(*tm));
  if(tm==NULL) return -1;
  tm->sec=sec;
  tm->Tim_Handler=func;
  tm->arg=arg;
  pool[pos]=tm;
  return 0;
}






