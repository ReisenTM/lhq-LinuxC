#ifndef CLIENT_H__
#define CLIENT_H__
#define DEFAULT_PLAYER "/usr/bin/mpg123 - > /dev/null"

struct client_conf_st {
  char *rcvport;
  char *mgroup;
  char *player_cmd;
};

#endif // !CLIENT_H__
