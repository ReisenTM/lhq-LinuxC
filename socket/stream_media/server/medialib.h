#ifndef MEDIALIB_H__
#define MEDIALIB_H__
#include "../include/proto.h"
#include <stddef.h>
#include <sys/types.h>
struct mlib_listinfo_t {
  chnid_t chnid;
  char *desc;
};

int mlisb_getchnlist(struct mlib_listinfo_t **, int *);
int mlib_freechnlist(struct mlib_listinfo_t *);

ssize_t mlib_readchn(chnid_t, void *, size_t);

#endif
