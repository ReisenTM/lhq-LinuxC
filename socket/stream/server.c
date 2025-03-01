#include "proto.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#define PTHSIZE 128
static void *Server_SendData(int nfd) {
  long long tm = (long long)time(NULL);
  char tmpbuf[BUFSIZE] = {0};
  int len = sprintf(tmpbuf, "%lld\n", tm);
  if (send(nfd, tmpbuf, len, 0) < 0) {
    perror("send()");
    exit(EXIT_FAILURE);
  }
  return NULL;
}
int main(int argc, char *argv[]) {
  pthread_t tid[PTHSIZE];
  struct sockaddr_in localAddr, dataPack;
  char buf[ADDRSIZE] = {0};
  // 创建socket
  int sockfd = socket(AF_INET, SOCK_STREAM, 0); // 创建流式传输socket
  if (sockfd < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }
  // socket绑定结构体初始化
  memset(&localAddr, 0, sizeof(localAddr));
  localAddr.sin_port = htons(atoi(SERVERPORT));
  localAddr.sin_family = AF_INET;
  inet_pton(AF_INET, "0.0.0.0", &localAddr.sin_addr);

  // 绑定地址
  if (bind(sockfd, (void *)&localAddr, sizeof(localAddr)) <
      0) // 绑定socket到对应地址和端口
  {
    perror("bind()");
    exit(EXIT_FAILURE);
  }

  // 监听地址
  if (listen(sockfd, QUEUESIZE) < 0) {
    perror("listen()");
    exit(EXIT_FAILURE);
  }

  // 收发数据
  socklen_t packSize = sizeof(dataPack);
  int pthreadID = 0; // 初始化任务累计数量
  while (1) {
    int nfd =
        accept(sockfd, (void *)&dataPack, &packSize); // 收到连接请求，获得fd
    inet_ntop(AF_INET, &dataPack.sin_addr, buf, ADDRSIZE);
    printf("connection from address %s:%d\n", buf, ntohs(dataPack.sin_port));
    Server_SendData(nfd);
    close(nfd);
  }
  // 关闭
  close(sockfd);

  return EXIT_SUCCESS;
}
