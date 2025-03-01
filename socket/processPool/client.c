#include "proto.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage...\n");
    exit(EXIT_FAILURE);
  }
  // 获取socket
  struct sockaddr_in rAddr;
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  // bind()
  memset(&rAddr, 0, sizeof(rAddr));
  rAddr.sin_port = htons(atoi(SERVERPORT));
  rAddr.sin_family = AF_INET;
  inet_pton(AF_INET, argv[1], &rAddr.sin_addr);
  // 建立连接
  if (connect(sockfd, (void *)&rAddr, sizeof(rAddr)) < 0) {
    perror("connect\n");
    exit(EXIT_FAILURE);
  }
  long long tim;
  FILE *fp = fdopen(sockfd, "r+");
  if (fp == NULL) {
    perror("fdopen\n");
    exit(EXIT_FAILURE);
  }
  // 从反馈的信息里读取时间
  if (fscanf(fp, "%lld", &tim) < 0) {
    fprintf(stderr, "fscanf()\n");
    exit(EXIT_FAILURE);
  } else {
    fprintf(stdout, "Time:%lld\n", tim);
  }

  close(sockfd);
  return EXIT_SUCCESS;
}
