#include "proto.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#define PTHSIZE 128
static void *Server_SendData(void *fd) {
  int *nfd = (int *)fd;
  long long tm = (long long)time(NULL);
  char tmpbuf[BUFSIZE] = {0};
  int len = sprintf(tmpbuf, "%lld\n", tm);
  if (send(*nfd, tmpbuf, len, 0) < 0) {
    printf("fd now:%d\n", *nfd);
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

  // 取消原地址绑定
  int val = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
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
  int nfd[PTHSIZE];
  while (1) {
    nfd[pthreadID] =
        accept(sockfd, (void *)&dataPack, &packSize); // 收到连接请求，获得fd
    if (nfd < 0) {
      perror("accept()");
      exit(EXIT_FAILURE);
    }
    inet_ntop(AF_INET, &dataPack.sin_addr, buf, ADDRSIZE);
    printf("connection from address %s:%d\n", buf, ntohs(dataPack.sin_port));
    int err =
        pthread_create(&tid[pthreadID], NULL, Server_SendData, &nfd[pthreadID]);
    if (err) {
      fprintf(stderr, "pthread_create");
      exit(EXIT_FAILURE);
    } else {
      pthreadID++;
    }
  }
  for (int i = 0; i < pthreadID; i++) {
    pthread_join(tid[i], NULL);
    close(nfd[i]);
  }
  // 关闭
  pthreadID = 0;
  close(sockfd);

  return EXIT_SUCCESS;
}
