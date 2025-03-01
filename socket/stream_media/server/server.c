#include "medialib.h"
#include "server_conf.h"
#include "thr_channel.h"
#include "thr_list.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <net/if.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
/*
 *  -M 指定多播组
 *  -P 指定端口
 *  -F 前台运行
 *  -D 指定媒体库位置
 *  -H 查看帮助
 *  -I 指定网络设备(网卡)
 * */
int serverfd;
struct sockaddr_in sndaddr;
static struct mlib_listinfo_t *list;

struct server_conf_st server_conf = {.rcvport = DEFAULT_RCVPORT,
                                     .ifname = DEFAULT_IF,
                                     .media_dir = DEFAULT_MEDIADIR,
                                     .mgroup = DEFAULT_MGROUP,
                                     .runmode = RUN_FOREGROUND};

static void Server_ShowHelp() {
  printf("-M 指定多播组\n");
  printf("-P 指定端口\n");
  printf("-F 前台运行\n");
  printf("-D 指定媒体库位\n");
  printf("-I 指定网络设备\n");
}
static int Server_Daemonize() {
  pid_t pid = fork();
  if (pid < 0) {
    // perror("fork()");
    syslog(LOG_ERR, "fork failed:%s", strerror(errno));
    return -1;
  }
  if (pid > 0)
    exit(0);
  // child
  int fd = open("/dev/null", O_RDWR);
  if (fd < 0) {
    // perror("open");
    syslog(LOG_WARNING, "open();%s", strerror(errno));
    exit(-2);
  } else {
    dup2(fd, 0);
    dup2(fd, 1);
    dup2(fd, 2);
    if (fd > 2)
      close(fd);
    chdir("/"); // 将当前工作路径改变为一个一定有的路径
    umask(0);
    setsid();
    return 0;
  }
}
static void Server_DaemonExit(int s) {
  thr_list_destroy();
  thr_channel_destroyall();
  mlib_freechnlist(list);
  syslog(LOG_WARNING, "signal %d caught,exit now.", s);
  closelog();
  exit(EXIT_SUCCESS);
}
static int Server_SocketInit() {
  struct ip_mreqn mreq;
  serverfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (serverfd < 0) {
    syslog(LOG_ERR, "socket():%s", strerror(errno));
    exit(EXIT_FAILURE);
  }
  inet_pton(AF_INET, server_conf.mgroup, &mreq.imr_multiaddr);
  inet_pton(AF_INET, "0,0,0,0", &mreq.imr_address);
  mreq.imr_ifindex = if_nametoindex(server_conf.ifname);
  if (setsockopt(serverfd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) <
      0) {
    syslog(LOG_ERR, "setsockopt():%s", strerror(errno));
    exit(EXIT_FAILURE);
  }
  // bind()
  sndaddr.sin_family = AF_INET;
  sndaddr.sin_port = htons(atoi(server_conf.rcvport));
  inet_pton(AF_INET, server_conf.mgroup, &sndaddr.sin_addr);
  return 0;
}
int main(int argc, char *argv[]) {
  // 写系统日志
  openlog("netradio", LOG_PID | LOG_PERROR, LOG_DAEMON);

  // 定义打断信号
  struct sigaction sa;
  sa.sa_handler = Server_DaemonExit;
  sigemptyset(&sa.sa_mask);
  sigaddset(&sa.sa_mask, SIGINT);
  sigaddset(&sa.sa_mask, SIGTERM);
  sigaddset(&sa.sa_mask, SIGQUIT);
  sigaction(SIGTERM, &sa, NULL);
  sigaction(SIGQUIT, &sa, NULL);
  sigaction(SIGINT, &sa, NULL);

  /*命令行分析*/
  while (1) {
    char ch = getopt(argc, argv, "M:P:FD:HI:");
    if (ch < 0)
      break;
    switch (ch) {
    case 'M':
      server_conf.mgroup = optarg;
      break;
    case 'P':
      server_conf.rcvport = optarg;
      break;
    case 'F':
      server_conf.runmode = RUN_FOREGROUND;
      break;
    case 'D':
      server_conf.media_dir = optarg;
      break;
    case 'H':
      Server_ShowHelp();
      exit(EXIT_SUCCESS);
    case 'I':
      server_conf.ifname = optarg;
      break;
    default:
      printf("Input not match\n");
      continue;
    }
  }

  /*守护进程实现*/
  if (server_conf.runmode == RUN_DAEMON) {
    if (Server_Daemonize() != 0)
      perror("Daemonize()");
  } else if (server_conf.runmode == RUN_FOREGROUND) {
    /*do some thing*/
  } else {
    // fprintf(stderr, "unrecognized runmode\n");
    syslog(LOG_ERR, "EINVAL server_conf.runmode");
    exit(EXIT_FAILURE);
  }
  /*socket初始化*/
  Server_SocketInit();
  /*获取频道信息*/
  int list_size;
  int err;
  err = mlisb_getchnlist(&list, &list_size);
  /*创建节目单线程*/
  thr_list_create(list, list_size);
  /*创建频道线程 */
  int count = 0;
  for (int count = 0; count < list_size; count++) {
    err = thr_channel_create(list + count);
    if (err) {
      fprintf(stderr, "thr_channel_create():%s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }
  syslog(LOG_DEBUG, "%d channel threads created", count);

  while (1) {
    pause();
  }
  // closelog();
  // return EXIT_SUCCESS;
}
