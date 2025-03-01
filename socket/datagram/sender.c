#include "proto.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
void Test_Init(struct msg_st *msg) { // 测试数据初始化
  strcpy(msg->name, "ZhangHao");
  msg->chinese = htonl(rand() % 100);
  msg->math = htonl(rand() % 100);
}
int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage..\n");
    exit(EXIT_FAILURE);
  }
  struct sockaddr_in PassAddr;
  struct msg_st msg;
  Test_Init(&msg);
  memset(&PassAddr, 0, sizeof(PassAddr));
  PassAddr.sin_port = htons(atoi(RCVPORT));
  PassAddr.sin_family = AF_INET;
  inet_pton(AF_INET, argv[1], &PassAddr.sin_addr);
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  // bind
  if (sendto(sockfd, &msg, sizeof(msg), 0, (void *)&PassAddr,
             sizeof(PassAddr))) {
    perror("sendto");
    exit(EXIT_FAILURE);
  }
  puts("send ok!");

  close(sockfd);

  return EXIT_SUCCESS;
}
