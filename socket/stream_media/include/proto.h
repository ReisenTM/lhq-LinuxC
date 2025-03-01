#ifndef PROTO_H__
#define PROTO_H__
#include "typeset.h"
#include <stdint.h>
#include <sys/types.h>
#define DEFAULT_RCVPORT "1997"
#define DEFAULT_MGROUP "224.2.2.2"

#define CHNNUM 100

#define CHNLIST 0 // 节目单频道

#define MINCHNID 1
#define MAXCHNID (MINCHNID + CHNNUM - 1)

#define MSG_CHNPACK_MAX (65536 - 20 - 8)
#define MAX_DATA (MSG_CHNPACK_MAX - sizeof(chnid_t))

#define MSG_LISTPACK_MAX (65536 - 20 - 8)
#define MAX_LIST (MSG_LISTPACK_MAX - sizeof(chnid_t))
// 频道结构体
struct msg_channel_st {

  chnid_t chnid;   /*必须在MIN-MAX之间*/
  uint8_t data[1]; // uint8_t表示该不定长数组以字节为单位
} __attribute__((packed));

struct msg_listinfo_t {
  chnid_t chnid;
  uint16_t len; // 变长套变长，包没有明显边界，所以让其自述长度
  char desc[1];
  // uint8_t desc[1];
} __attribute__((packed));

// 节目单结构体
struct msg_list_st {
  chnid_t chnid /*必须为CHNLIST*/;
  struct msg_listinfo_t list[1];
} __attribute__((packed));

#endif // !PROTO_H__
