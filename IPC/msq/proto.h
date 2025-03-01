#ifndef PROTO_H_
#define PROTO_H_
#define KEYPATH "/etc/services"
#define KEYID 'a'
#define NAMESIZE 32
struct msgst {
  long msgtyp;
  char name[NAMESIZE];
  int score;
};
#endif // !PROTO_H_
