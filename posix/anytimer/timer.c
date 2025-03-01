#include "timer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
typedef void (*fun)(void *);
#define MAXSIZE 1024
static pthread_t timer;
static pthread_t checker;
struct tim_t {
  fun f;
  int time;
  int pos;
  void *arg;
};
static struct tim_t *arr[MAXSIZE];
static int token;
static pthread_mutex_t mut;
static pthread_cond_t cond;

static void destoryTimer(tim_t *ptr) {
  struct tim_t *tmp = ptr;
  free(tmp);
}
// 一个线程用来扫描并执行,定义一个token 有一个token扫描一次
static void checkArr() {
  while (token <= 0)
    pthread_cond_wait(&cond, &mut);

  for (int i = 0; i < MAXSIZE; i++) {
    if (arr[i] == NULL)
      continue;
    if (arr[i]->time == 0 && arr[i] != NULL) {
      arr[i]->f(arr[i]->arg);
      destoryTimer(arr[i]);
    }
  }
  token--;
}

static void *check_routine(void *p) {
  while (1) {
    checkArr();
  }
}

// 一个线程用来计时，每一秒增加一个token
static void subTime() {
  for (int i = 0; i < MAXSIZE; i++) {
    if (arr[i] != NULL) {
      arr[i]->time--;
    }
  }
}

static void *tim_routine(void *p) {
  while (1) {
    sleep(1);
    pthread_mutex_lock(&mut);
    subTime();
    token++;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mut);
    // usleep(1000000);
  }
  return NULL;
}
/*
 *作用：放出一个线程用来计时，另一个线程每一秒扫描一次数组看是否有计时器到时
 *参数：无
 * */

int initTimer(void) {
  pthread_mutex_init(&mut, NULL);
  pthread_cond_init(&cond, NULL);
  int err = pthread_create(&timer, NULL, tim_routine, NULL);
  if (err)
    return -1;
  int err2 = pthread_create(&checker, NULL, check_routine, NULL);
  if (err2)
    return -2;
  return 0;
}

static int findPos(void) {
  for (int i = 0; i < MAXSIZE; i++) {
    if (arr[i] == NULL)
      return i;
  }
  return -1;
}
int setTimer(int time, fun func, void *arg) {
  struct tim_t *tim = malloc(sizeof(*tim));
  if (tim == NULL) {
    return -1; // 内存不足
  }
  tim->f = func;
  tim->arg = arg;
  tim->time = time;
  tim->pos = findPos();
  if (tim->pos < 0) {
    return -2; // 没找到空位
  }
  arr[tim->pos] = tim;
  return 0;
}
int delTimer() {
  pthread_mutex_destroy(&mut);
  pthread_cond_destroy(&cond);
  pthread_join(timer, NULL);
  pthread_join(checker, NULL);
  return 0;
}
