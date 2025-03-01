#include "thr_channel.h"
#include "../include/proto.h"
#include "medialib.h"
#include "server_conf.h"
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/syslog.h>
#include <syslog.h>

struct thr_channel_ent_st {
  chnid_t chnid;
  pthread_t pid; // 用来保存创建好的频道线程
};

struct thr_channel_ent_st thr_channel[CHNNUM]; // 保存创建好的频道线程

static int tid_pos;
static void *thr_channel_snder(void *ptr) {
  struct msg_channel_st *sndpack;
  struct mlib_listinfo_t *ent = ptr;
  sndpack = malloc(sizeof(struct msg_channel_st) *
                   MSG_CHNPACK_MAX); // 不定长内容，暂时分配最大内存
  if (sndpack == NULL) {
    syslog(LOG_ERR, "malloc:%s", strerror(errno));
    exit(EXIT_FAILURE);
  }

  sndpack->chnid = ent->chnid;
  while (1) {
    int len = mlib_readchn(sndpack->chnid, sndpack->data,
                           MAX_DATA); // 读取频道信息，获取长度
    if (sendto(serverfd, sndpack, len + sizeof(chnid_t), 0, (void *)&sndaddr,
               sizeof(sndaddr)) < 0) {
      syslog(LOG_ERR, "sendto():%s", strerror(errno));
      exit(EXIT_FAILURE);
    }
    sched_yield();
  }
  pthread_exit(NULL);
}
int thr_channel_create(struct mlib_listinfo_t *ptr) {
  int err =
      pthread_create(&thr_channel[tid_pos].pid, NULL, thr_channel_snder, ptr);
  if (err) {
    syslog(LOG_ERR, "pthread_create();%s", strerror(errno));
    return -err;
  }
  thr_channel[tid_pos].chnid = ptr->chnid;
  tid_pos++;
  return 0;
}
// 销毁指定频道线程
int thr_channel_destroy(struct mlib_listinfo_t *ptr) {
  for (int i = 0; i < CHNNUM; i++) {
    if (thr_channel[i].chnid == ptr->chnid) {
      pthread_cancel(thr_channel[i].pid);
      pthread_join(thr_channel[i].pid, NULL);
      thr_channel[i].chnid = -1;
      break;
    }
  }
  return 0;
}

// 销毁所有的频道线程
int thr_channel_destroyall(void) {
  for (int i = 0; i < CHNNUM; i++) {
    if (thr_channel[i].chnid > 0) {
      if (pthread_cancel(thr_channel[i].pid) < 0) {
        syslog(LOG_ERR, "pthread_cancel():thr thread of channel:%ld",
               thr_channel[i].pid);
        return -ESRCH;
      }
      pthread_join(thr_channel[i].pid, NULL);
      thr_channel[i].chnid = -1;
    }
  }
  return 0;
}
