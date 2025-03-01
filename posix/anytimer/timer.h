#ifndef TIMER_H_
#define TIMER_H_
typedef void (*fun)(void *);
typedef void tim_t;
int initTimer(void);
int setTimer(int time, fun f, void *arg);
// 用户需要：设置定时时间+调用函数
// 需要考虑：结构体，销毁
int delTimer();

#endif // !a
