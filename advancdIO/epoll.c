#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/poll.h>
#include <sys/select.h>
#include <unistd.h>
#define TTY1 "/dev/tty1"
#define TTY2 "/dev/tty2"
#define BUFSIZE 1024
enum {
  STATE_WR = 1,
  STATE_RD,
  STATE_DIVIDE,
  STATE_END,
  STATE_ERR,
};

struct fsm_st {
  int state;
  int fds;
  int fdd;
  char buf[BUFSIZE];
  int len;
  int pos;
  char *strerr;
};

void branch(struct fsm_st *fsm) {
  switch (fsm->state) {
  case STATE_RD:
    fsm->len = read(fsm->fds, fsm->buf, BUFSIZE);
    if (fsm->len == 0)
      fsm->state = STATE_END;
    else if (fsm->len < 0) {
      if (errno == EAGAIN)
        fsm->state = STATE_RD;
      else {
        fsm->strerr = "read()";
        fsm->state = STATE_ERR;
      }
    } else {
      fsm->state = STATE_WR;
      fsm->pos = 0;
    }
    break;
  case STATE_WR:
    int ret = write(fsm->fdd, fsm->buf + fsm->pos, BUFSIZE);
    if (ret < 0) {
      if (errno == EAGAIN)
        fsm->state = STATE_WR;
      else {
        fsm->strerr = "write()";
        fsm->state = STATE_ERR;
      }
    } else if (ret > 0) {
      fsm->len -= ret;
      fsm->pos += ret;
      if (ret == 0)
        fsm->state = STATE_RD;
      else {
        fsm->state = STATE_WR;
      }
    }
    break;
  case STATE_END:
    break;
  case STATE_ERR:
    break;
  default:
    abort();
  }
}
static int max(int f1, int f2) { return f1 > f2 ? f1 : f2; }
// 数据中继
void transport(int fd1, int fd2) {
  int fd1_save = fcntl(fd1, F_GETFD);
  int fd2_save = fcntl(fd2, F_GETFD);
  int err1 = fcntl(fd1, F_SETFD, fd1_save | O_NONBLOCK);
  int err2 = fcntl(fd2, F_SETFD, fd2_save | O_NONBLOCK);
  if (err1 || err2) {
    fprintf(stderr, "fcntl wrong\n");
    exit(-1);
  }
  struct fsm_st fsm12, fsm21;
  fsm12.fds = fd1;
  fsm12.fdd = fd2;
  fsm12.state = STATE_RD;
  fsm21.fds = fd2;
  fsm21.fdd = fd1;
  fsm21.state = STATE_RD;
  int epoll_fd = epoll_create(10);
  if (epoll_fd < 0) {
    perror("epool");
    exit(-1);
  }
  struct epoll_event EPevent;
  EPevent.data.fd = fd1;
  EPevent.events = 0; // 位图
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd1, &EPevent);
  EPevent.data.fd = fd2;
  EPevent.events = 0; // 位图
  epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd2, &EPevent);

  while (fsm12.state != STATE_END || fsm21.state != STATE_END) {
    EPevent.data.fd = fd1;
    EPevent.events = 0;
    // 设置监控
    if (fsm12.state == STATE_RD)
      EPevent.events |= EPOLLIN;
    if (fsm12.state == STATE_WR)
      EPevent.events |= EPOLLOUT;
    EPevent.data.fd = fd2;
    EPevent.events = 0;
    if (fsm21.state == STATE_RD)
      EPevent.events |= EPOLLIN;
    if (fsm21.state == STATE_WR)
      EPevent.events |= EPOLLOUT;
    // 监控
    if (fsm12.state < STATE_DIVIDE || fsm21.state < STATE_DIVIDE) {
      while (epoll_wait(epoll_fd, &EPevent, 1, -1) < 0) {
        if (errno == EINTR)
          continue;
        perror("epoll_wait()");
        exit(EXIT_FAILURE);
      }
    }
    // 监控结果反馈
    if (EPevent.data.fd == fd1 && EPevent.events & POLLIN ||
        EPevent.data.fd == fd2 && EPevent.events & POLLOUT ||
        fsm12.state > STATE_DIVIDE)
      branch(&fsm12);
    if (EPevent.data.fd == fd2 && EPevent.events & POLLIN ||
        EPevent.data.fd == fd1 && EPevent.events & POLLOUT ||
        fsm21.state > STATE_DIVIDE)
      branch(&fsm21);
  }
  fd1 = fcntl(fd1, F_SETFD, fd1_save);
  fd2 = fcntl(fd2, F_SETFD, fd2_save);
}

int main(int argc, char *argv[]) {
  int fd1, fd2;
  fd1 = open(TTY1, O_RDWR);
  if (fd1 < 0) {
    perror("open ");
    exit(-1);
  }
  fd2 = open(TTY2, O_RDWR);
  if (fd1 < 0) {
    close(fd1);
    perror("open ");
    exit(-1);
  }
  transport(fd1, fd2);
  close(fd1);
  close(fd2);
  return EXIT_SUCCESS;
}
