
#include "proto.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
static struct msgst mst;
static void Data_Input() {
  mst.msgtyp = 1;
  char tmpbuf[NAMESIZE];
  puts("please input name");
  fgets(tmpbuf, NAMESIZE, stdin);
  int p = strcspn(tmpbuf, "\n");
  tmpbuf[p] = '\0';
  strcpy(mst.name, tmpbuf);
  puts("please input score");
  fgets(tmpbuf, NAMESIZE, stdin);
  mst.score = atoi(tmpbuf);
}
int main(int argc, char *argv[]) {
  key_t key = ftok(KEYPATH, KEYID);
  int msgid = msgget(key, 0);
  while (1) {
    Data_Input();
    msgsnd(msgid, &mst, sizeof(mst) - sizeof(long), 0);
  }
  // strcpy(mst.name, "tom");
  // mst.score = 87;
  // mst.msgtyp = 1;
  // msgsnd(msgid, &mst, sizeof(mst) - sizeof(long), 0);

  return EXIT_SUCCESS;
}
