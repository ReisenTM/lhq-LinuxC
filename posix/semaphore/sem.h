#ifndef SEM_H__
#define SEM_H__
typedef void sem_t;
sem_t *Sem_Init(int n);
int Sem_Ret(sem_t *sem, int number);
int Sem_Sub(sem_t *sem, int number);
void Sem_Destroy(sem_t *sem);
#endif /* ifndef _SEM_H__ */
