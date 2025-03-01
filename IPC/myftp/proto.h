
#ifndef PROTO_H__
#define PROTO_H__
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define KEYPATH "/etc/services"
#define KEYNUM 'y'
#define MAXSIZE 128 // 一次最多传128字符
#define NAMESIZE 64 // 请求文件名大小
#define ALLEND 0
#define DONE 1
#define WORK 2
struct msg_st {
  long msgtyp; // 文件类型
  char data[MAXSIZE];
  char filename[NAMESIZE]; // 请求文件名
  int8_t flag;             // 若end设为0   done为1
};

#endif /* ifndef MACRO */
