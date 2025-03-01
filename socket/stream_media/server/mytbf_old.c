#include "mytbf.h"
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
// real st
struct mytbf_st {
  int cps;
  int pos;
  int token;
  int limit;
  pthread_mutex_t mut;
  pthread_cond_t cond;
};

#define DEBUG

static pthread_t pid;
static struct mytbf_st *job[MYTBF_MAX];
static pthread_mutex_t mut_job =
    PTHREAD_MUTEX_INITIALIZER; // job数组也是共享资源，也要上互斥锁
static pthread_once_t init_once = PTHREAD_ONCE_INIT;

static int find_pos_unlock() {
  for (int i = 0; i < MYTBF_MAX; i++) {
    if (job[i] == NULL)
      return i;
  }
  return -1;
}
static void module_unload() {
  pthread_cancel(pid);
  pthread_join(pid, NULL);
  for (int i = 0; i < MYTBF_MAX; i++) {
    free(job[i]);
  }
  return;
}
// 实现流控
static void alrm_handle(int sig) {
  pthread_mutex_lock(&mut_job);
  for (int i = 0; i < MYTBF_MAX; ++i) {
    if (job[i] != NULL) {
      pthread_mutex_lock(&job[i]->mut);
      job[i]->token += job[i]->cps;
      if (job[i]->token > job[i]->limit) {
        job[i]->token = job[i]->limit;
      }
      pthread_cond_broadcast(&job[i]->cond); // 惊群
      pthread_mutex_unlock(&job[i]->mut);
    }
  }
  pthread_mutex_unlock(&mut_job);
}

static void *thr_alrm(void *p) {
  struct itimerval tick;
  memset(&tick, 0, sizeof(tick));
  tick.it_value.tv_sec = 1;  // sec
  tick.it_value.tv_usec = 0; // micro sec.
  tick.it_interval.tv_sec = 1;
  tick.it_interval.tv_usec = 0;

  signal(SIGALRM, alrm_handle);
  setitimer(ITIMER_REAL, &tick, NULL);

  while (1) {
    pause();
  }
}

// static void *thr_alarm(void *p) {
//   while (1) {
//     pthread_mutex_lock(&mut_job);
//     for (int i = 0; i < MYTBF_MAX; i++) {
//       if (job[i] != NULL) {
//         pthread_mutex_lock(&job[i]->mut);
//         job[i]->token += job[i]->cps;
// #ifdef DEBUG
//         printf("job[%d] token:%d\n", i, job[i]->token);
//         printf("job[%d] cps:%d,limit:%d \n", i, job[i]->cps, job[i]->limit);
// #endif
//         if (job[i]->token > job[i]->limit)
//           job[i]->token = job[i]->limit;
//         pthread_cond_broadcast(&job[i]->cond);
//         pthread_mutex_unlock(&job[i]->mut);
//       }
//     }
//     pthread_mutex_unlock(&mut_job);
//     sleep(1);
//   }
// }
static void module_load() {
  int err = pthread_create(&pid, NULL, thr_alrm, 0);
  if (err) {
    fprintf(stderr, "pthread_create()\n");
    exit(EXIT_FAILURE);
  }
  atexit(module_unload);
}
mytbf_t *mytbf_init(int cps, int limit) {
  struct mytbf_st *me;
  // module_load();
  pthread_once(&init_once, module_load); // 只有第一次会调用
  me = malloc(sizeof(*me));
  if (me == NULL)
    return NULL;
  me->cps = cps;
  me->limit = limit;
  me->token = 0;
  pthread_mutex_init(&me->mut, NULL);
  pthread_cond_init(&me->cond, NULL);
  pthread_mutex_lock(&mut_job);
  me->pos = find_pos_unlock();
  job[me->pos] = me;
  if (me->pos < 0) {
    pthread_mutex_unlock(&mut_job);
    free(me);
    fprintf(stderr, "no usable pos\n");
    return NULL;
  }
  pthread_mutex_unlock(&mut_job);

  return me;
}

static int min(int num1, int num2) { return num1 < num2 ? num1 : num2; }
int mytbf_fetchtoken(mytbf_t *ptr, int num) {
  int n;
  struct mytbf_st *tmp = ptr;
  pthread_mutex_lock(&tmp->mut);
  while (tmp->token <= 0)
    pthread_cond_wait(&tmp->cond, &tmp->mut);
  n = min(tmp->token, num);
  tmp->token -= n;
  pthread_mutex_unlock(&tmp->mut);
  return n;
}

int mytbf_returntoken(mytbf_t *ptr, int num) {
  int n;
  struct mytbf_st *tmp = ptr;
  pthread_mutex_lock(&tmp->mut);
  tmp->token += num;
  if (num > tmp->limit)
    tmp->token = tmp->limit;
  pthread_cond_broadcast(&tmp->cond);
  pthread_mutex_unlock(&tmp->mut);
  return 0;
}

int mytbf_destroy(mytbf_t *ptr) {
  struct mytbf_st *tmp = ptr;
  pthread_mutex_lock(&mut_job);
  job[tmp->pos] = NULL;
  pthread_mutex_unlock(&mut_job);
  pthread_mutex_destroy(&tmp->mut);
  pthread_cond_destroy(&tmp->cond);
  free(tmp);
  return 0;
}

int mytbf_checktoken(mytbf_t *ptr) {
  int token_left = 0;
  struct mytbf_st *me = ptr;
  pthread_mutex_lock(&me->mut);
  token_left = me->token;
  pthread_mutex_unlock(&me->mut);
  return token_left;
}
