#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "tokenBucket.h"
#include <stdint.h>
#define MAXSIZE 1024
typedef void (*sighandler_t)(int);
static int8_t initflag;
static sighandler_t handler_save; 
typedef void mytb_t;
typedef struct 
{
    int cps;
    int burst;
    int token;    
    int pos; 
}mytb_st;
static mytb_st* job[MAXSIZE];//数组用来存放不同·令牌桶

static int find_vacant_pos(mytb_st* job[]){ //找空位，存放令牌桶结构体指针
    for(int i=0;i<MAXSIZE;i++){
        if(job[i]==NULL) return i;
    }
    return -1;
}
static void alrm_handler(int s){
    alarm(1);
    for(int i=0;i<MAXSIZE;i++){
        if(job[i]!=NULL){
            job[i]->token+=job[i]->cps;
            if(job[i]->token>=job[i]->burst)
                job[i]->token=job[i]->burst;
        }
    }
}
static void module_unload(void){
    //signal(SIGALRM,SIG_DFL); 不确定之前的函数是否是默认函数
    signal(SIGALRM,handler_save);
    alarm(0);
    for(int i=0;i<MAXSIZE;i++){
        free(job[i]);
    }
    exit(0);
}
static void module_load(void){
    handler_save=signal(SIGALRM,alrm_handler);
    alarm(1);
    atexit(module_unload);
}

mytb_t* Mytb_Init(int cps,int burst){
    int pos=find_vacant_pos(job);
    if(pos<0){
        return NULL;
    }
    mytb_st* mt=malloc(sizeof(*mt));
    if(mt==NULL){
        return NULL;//如果分配空间失败,返回空指针
    }
    job[pos]=mt;//存放令牌桶结构体指针
    //发出首次指令
    if(!initflag){
        module_load();
        initflag=1;
    }

    mt->pos=pos;
    mt->burst=burst;
    mt->cps=cps;
    mt->token=0;
    return mt;
}
//取token
int Mytb_Fetchtoken(mytb_t* tb,int n){
    mytb_st* tmp=tb;
    int res=0;
    if(n<0)  return -1;
    while(tmp->token<=0) pause();//不要用if,因为其他信号可能打断pause
    if(tmp->token>0) res=(tmp->token<n?tmp->token:n);
    tmp->token-=res;
    return res;
}
//还token
int Mytb_Returntoken(mytb_t* tb,int n){
    mytb_st* tmp=tb;
    if(n<0)  return -1;
    tmp->token+=n;
    if(tmp->token>tmp->burst) tmp->token=tmp->burst;
    return n;
}
int Mytb_Destroy(mytb_t* tb){
    if(tb==NULL) return -1;
    mytb_st* tmp=tb;
    job[tmp->pos]=NULL;
    free(tmp);
    return 0;
}

