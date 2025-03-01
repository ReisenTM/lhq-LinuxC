#ifndef PROTO_H__
#define PROTO_H__
#include <stdint.h>
#define RCVPORT "1993"
#define NAMESIZE 11
#define BUFSIZE 1024
struct msg_st {
  uint8_t name[NAMESIZE];
  uint32_t math;
  uint32_t chinese;
} __attribute__((packed));

#endif // !PROTO_H__
