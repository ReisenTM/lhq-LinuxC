#ifndef TIMER_H__
#define TIMER_H__
typedef void TimHandler_t(void *);
void initTimer();

int setTimer(int sec,TimHandler_t* func,void *arg);
#endif // !TIMER_H__


