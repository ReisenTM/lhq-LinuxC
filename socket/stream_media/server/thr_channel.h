#ifndef THR_CHANNEL_H
#define THR_CHANNEL_H

#include "medialib.h"
int thr_channel_create(struct mlib_listinfo_t *);
int thr_channel_destroy(struct mlib_listinfo_t *);
int thr_channel_destroyall(void);

#endif // !THR_CHANNEL_H
