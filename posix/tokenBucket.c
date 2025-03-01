#include "tokenBucket.h"
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAXSIZE 1024
static pthread_t ttid;
// typedef void (*sighandler_t)(int);
static int8_t initflag;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
// static sighandler_t handler_save;
typedef void mytb_t;
typedef struct {
  int cps;
  int burst;
  int token;
  int pos;
  pthread_mutex_t tokenMut;
} mytb_st;
static mytb_st *job[MAXSIZE]; // 数组用来存放不同·令牌桶
static pthread_once_t once = PTHREAD_ONCE_INIT;
static int find_vacant_pos_unlock(
    mytb_st *job[]) { // 找空位，存放令牌桶结构体指针(该函数并发时应避免竞争)
  for (int i = 0; i < MAXSIZE; i++) {
    if (job[i] == NULL)
      return i;
  }
  return -1;
}
static void alrm_handler(int s) {
  // alarm(1);
  while (1) {
    pthread_mutex_lock(&mut); // 锁住整个数组
    for (int i = 0; i < MAXSIZE; i++) {
      if (job[i] != NULL) {
        pthread_mutex_lock(&job[i]->tokenMut); // 锁送单就token
        job[i]->token += job[i]->cps;
        if (job[i]->token >= job[i]->burst)
          job[i]->token = job[i]->burst;
        pthread_mutex_unlock(&job[i]->tokenMut);
      }
    }
    pthread_mutex_unlock(&mut);
    usleep(1000);
  }
}
static void module_unload(void) {
  // signal(SIGALRM,SIG_DFL); 不确定之前的函数是否是默认函数
  // signal(SIGALRM, handler_save);
  // alarm(0);
  pthread_t tid;
  pthread_cancel(tid);
  pthread_join(tid, NULL);
  for (int i = 0; i < MAXSIZE; i++) {
    if (job[i] != NULL)
      Mytb_Destroy(job[i]);
  }
  pthread_mutex_destroy(&mut);
  pthread_join(ttid, NULL);
  exit(0);
}
static void module_load(void) {
  //  handler_save=signal(SIGALRM,alrm_handler);
  // alarm(1);
  pthread_t tsid;
  pthread_create(&tsid, NULL, (void *)alrm_handler, NULL);
  atexit(module_unload);
}

mytb_t *Mytb_Init(int cps, int burst) {

  mytb_st *mt = malloc(sizeof(*mt));
  if (mt == NULL) {
    return NULL; // 如果分配空间失败,返回空指针
  }
  // // 发出首次指令
  // if (!initflag) {
  //   module_load();
  //   initflag = 1;
  // }
  pthread_once(&once, module_load);
  mt->burst = burst;
  mt->cps = cps;
  mt->token = 0;
  pthread_mutex_init(&mt->tokenMut, NULL); // token锁初始化
  pthread_mutex_lock(&mut); // 加锁，因为每个进程找空位时应该是避免竞争的
  int pos = find_vacant_pos_unlock(job);
  if (pos < 0) {
    pthread_mutex_unlock(&mut);
    free(mt);
    return NULL;
  }
  mt->pos = pos;
  job[pos] = mt; // 存放令牌桶结构体指针
  pthread_mutex_unlock(&mut);
  return mt;
}
// 取token
int Mytb_Fetchtoken(mytb_t *tb, int n) {
  mytb_st *tmp = tb;
  int res = 0;
  if (n < 0)
    return -1;
  pthread_mutex_lock(&tmp->tokenMut);
  while (tmp->token <= 0) { // 等待token
    pthread_mutex_unlock(&tmp->tokenMut);
    sched_yield();
    pthread_mutex_lock(&tmp->tokenMut);
  }
  if (tmp->token > 0)
    res = (tmp->token < n ? tmp->token : n);
  tmp->token -= res;
  pthread_mutex_unlock(&tmp->tokenMut);
  return res;
}
// 还token
int Mytb_Returntoken(mytb_t *tb, int n) {
  mytb_st *tmp = tb;
  if (n < 0)
    return -1;
  pthread_mutex_lock(&tmp->tokenMut);
  tmp->token += n;
  if (tmp->token > tmp->burst)
    tmp->token = tmp->burst;
  pthread_mutex_unlock(&tmp->tokenMut);
  return n;
}
int Mytb_Destroy(mytb_t *tb) {
  if (tb == NULL)
    return -1;
  mytb_st *tmp = tb;
  pthread_mutex_lock(&mut);
  job[tmp->pos] = NULL;
  pthread_mutex_unlock(&mut);
  free(tmp);
  return 0;
}
