
#include "proto.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
  struct sockaddr_in localAddr, PassAddr;
  struct msg_st msg;
  int sfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sfd < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }
  localAddr.sin_family = AF_INET;
  localAddr.sin_port = htons(atoi(RCVPORT));
  inet_pton(AF_INET, "0.0.0.0",
            &localAddr.sin_addr); // 把地址转为大整数存在结构体里(包括htos转化)
  if (bind(sfd, (void *)&localAddr, sizeof(localAddr)) < 0) {
    perror("bind()");
    exit(EXIT_FAILURE);
  }
  socklen_t socksize = sizeof(PassAddr);
  char addr_register[64]; // 临时存放传来的ip地址
  while (1) {
    recvfrom(sfd, &msg, BUFSIZE, 0, (void *)&PassAddr, &socksize);
    inet_ntop(AF_INET, &PassAddr.sin_addr, addr_register, 64);
    printf("---Message from %s:%d\n", addr_register,
           (ntohs(PassAddr.sin_port)));
    printf("NAME:%s\n", msg.name);
    printf("chinese:%d\n", ntohl(msg.chinese));
    printf("math:%d\n", ntohl(msg.math));
    printf("Data received\n");
  }
  close(sfd);
  return 0;
}
