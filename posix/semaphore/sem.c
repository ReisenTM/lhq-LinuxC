#include "sem.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
struct sem_t {
  int srcval;
  pthread_mutex_t mut;
  pthread_cond_t cond;
};
sem_t *Sem_Init(int n) {
  struct sem_t *sem = malloc(sizeof(*sem));
  if (sem == NULL) {
    exit(1);
    fprintf(stderr, "malloc\n");
  }
  sem->srcval = n;
  pthread_mutex_init(&sem->mut, NULL);
  pthread_cond_init(&sem->cond, NULL);
  return sem;
}
int Sem_Ret(sem_t *sem, int number) {
  struct sem_t *tmp = sem;
  pthread_mutex_lock(&tmp->mut);
  tmp->srcval += number;
  pthread_cond_broadcast(&tmp->cond); // 资源增加之后发信号通知所有等待线程
  pthread_mutex_unlock(&tmp->mut);
  return number;
}
int Sem_Sub(sem_t *sem, int number) {
  struct sem_t *tmp = sem;
  pthread_mutex_lock(&tmp->mut);
  while (tmp->srcval < number) { // 如果资源不够，等待
    pthread_cond_wait(&tmp->cond, &tmp->mut);
  }
  tmp->srcval -= number;
  pthread_mutex_unlock(&tmp->mut);

  return number;
}
void Sem_Destroy(sem_t *ptr) {
  struct sem_t *sem = ptr;
  pthread_mutex_destroy(&sem->mut);
  pthread_cond_destroy(&sem->cond);
  free(sem);
}
