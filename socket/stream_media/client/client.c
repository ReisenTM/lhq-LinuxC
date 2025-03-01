#include "client.h"
#include "../include/proto.h"
#include "../include/typeset.h"
#include <arpa/inet.h>
#include <bits/getopt_core.h>
#include <errno.h>
#include <getopt.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
/*
 *  -M --mgroup 显示多播组
 *  -P --port 显示接收端口
 *  -p --player 显示播放器
 *  -H --help 显示帮助
 * */

/*默认属性指定*/
struct client_conf_st client_conf = {.rcvport = DEFAULT_RCVPORT,
                                     .mgroup = DEFAULT_MGROUP,
                                     .player_cmd = DEFAULT_PLAYER};
static void showHelp() {
  printf("-P --port :指定端口\n");
  printf("-p --player :指定播放器\n");
  printf("-H --help :显示帮助\n");
  printf("-M --mgroup :指定多播组\n");
}
// 坚持写够n个字节
static ssize_t writen(int fd, const char *buf, size_t len) {
  int ret = 0, pos = 0;
  while (len > 0) {
    ret = write(fd, buf + pos, len);
    if (ret == 0)
      break;
    if (ret < 0) {
      if (errno == EINTR) // 如果write是因为意外被打断
        continue;
      perror("write");
      return -1;
    }
    len -= ret;
    pos += ret;
  }
  return pos;
}
int main(int argc, char *argv[]) {
  int index = 0;
  struct option argarr[] = {{"port", 1, NULL, 'P'},
                            {"mgroup", 1, NULL, 'M'},
                            {"player", 1, NULL, 'p'},
                            {"help", 0, NULL, 'H'},
                            {NULL, 0, NULL, 0}};
  /*命令行解析*/
  while (1) {
    char ch = getopt_long(argc, argv, "p:P:M:H", argarr, &index);
    if (ch < 0)
      break;
    switch (ch) {
    case 'P':
      client_conf.rcvport = optarg;
      break;
    case 'M':
      client_conf.mgroup = optarg;
      break;
    case 'p':
      client_conf.player_cmd = optarg;
      break;
    case 'H':
      showHelp();
      break;
    dafault:
      printf("please check input\n");
      abort(); // 看不懂内容，就自己发送结束信号，结束程序
      break;
    }
  }

  /*网络变量声明区*/
  struct sockaddr_in localaddr, remoteaddr, serveraddr;
  socklen_t serveraddr_len = sizeof(serveraddr),
            remoteaddr_len = sizeof(remoteaddr);
  struct ip_mreqn ipreq;
  /*获取socket*/
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket()");
    exit(EXIT_FAILURE);
  }

  // 设置加入多播组
  inet_pton(AF_INET, client_conf.mgroup, &ipreq.imr_multiaddr);
  inet_pton(AF_INET, "0,0,0,0", &ipreq.imr_address);
  ipreq.imr_ifindex = if_nametoindex("eth6");
  if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &ipreq, sizeof(ipreq)) <
      0) {
    perror("setsockopt");
    exit(EXIT_FAILURE);
  }

  int val;
  if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, &val, sizeof(val)) <
      0) {
    perror("setsockopt-loop");
    exit(EXIT_FAILURE);
  }

  // 绑定本地地址到socket
  memset(&localaddr, 0, sizeof(localaddr));
  localaddr.sin_family = AF_INET;
  localaddr.sin_port = htons(atoi(client_conf.rcvport));
  inet_pton(AF_INET, "0,0,0,0", &localaddr.sin_addr);

  if (bind(sockfd, (void *)&localaddr, sizeof(localaddr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  // 创建管道
  int pfd[2]; // 一端读(0) 一端写(1)
  if (pipe(pfd) < 0) {
    perror("pipe");
    exit(EXIT_FAILURE);
  }

  // 创建子进程
  pid_t pid;
  pid = fork();
  if (pid == 0) {  //*子进程：调用解码器 */
    close(sockfd); // 关闭无用文件描述符
    close(pfd[1]);

    dup2(pfd[0], 0); // 把管道读定向到标准输入(配合mpeg123从标准输入读取)
    if (pfd[0] > 0)
      close(pfd[0]);

    execl("/bin/sh", "sh", "-c", client_conf.player_cmd,
          NULL); // 利用shell调用解码器
    // 如果exec失败
    perror("exec");
    exit(EXIT_FAILURE);
  } else if (pid > 0) { //*父进程：从网络收包，发送给子进程*/
    // 收节目单包
    struct msg_list_st *msg_list;
    msg_list = malloc(MSG_LISTPACK_MAX);
    if (msg_list == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }
    int len = 0;
    while (1) {
      len = recvfrom(sockfd, msg_list, MSG_LISTPACK_MAX, 0, (void *)&serveraddr,
                     &serveraddr_len);
      if (len < sizeof(struct msg_list_st)) {
        fprintf(stderr, "message is too small\n");
        continue;
      }
      if (msg_list->chnid != CHNLIST) {
        fprintf(stderr, "chnid is not matched\n");
        continue;
      }
      break;
    }
    // 打印节目单
    struct msg_listinfo_t *pos;

    for (pos = msg_list->list; (char *)pos < (((char *)msg_list) + len);
         pos = (void *)((char *)pos + ntohs(pos->len))) {
      // char*是为了让pos以字节为单位移动
      printf("channel%d : %s\n ", pos->chnid, pos->desc);
    }
    free(msg_list);

    // 选频道
    int chosenid = 0, ret = 0;
    while (1) {
      ret = scanf("%d", &chosenid);
      if (ret != 1)
        exit(EXIT_FAILURE);
    }

    // 收频道包,发送给子进程
    struct msg_channel_st *msg_channel;
    msg_channel = malloc(MSG_CHNPACK_MAX);
    if (msg_channel == NULL) {
      perror("malloc");
      exit(EXIT_FAILURE);
    }

    while (1) {
      // 收包
      int len = recvfrom(sockfd, msg_channel, MSG_CHNPACK_MAX, 0,
                         (void *)&remoteaddr, &remoteaddr_len);
      // 如果地址不对，跳到下次收包
      if (remoteaddr.sin_addr.s_addr != serveraddr.sin_addr.s_addr) {
        fprintf(stderr, "address is not match\n");
        continue;
      }
      // 如果收到的数据过小
      if (len < sizeof(struct msg_channel_st)) {
        fprintf(stderr, "size of message too small\n");
        continue;
      }
      // 如果收到的内容与想要介绍的包一致
      if (msg_channel->chnid == chosenid) {
        fprintf(stdout, "accept msg:%d received \n", msg_channel->chnid);
        // 把收到的频道包输出到标准输出上，以供mpeg读取
        if (writen(pfd[1], msg_channel->data, len - sizeof(chnid_t)) < 0) {
          fprintf(stderr, "writen\n");
          exit(EXIT_FAILURE);
        }
      }
    }
    free(msg_channel);
    close(sockfd);

  } else {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}
