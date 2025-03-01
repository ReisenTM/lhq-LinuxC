#ifndef MYTBF_H__
#define MYTBF_H__
#define MYTBF_MAX 1024
typedef void mytbf_t;

mytbf_t *mytbf_init(int cps, int limit);

int mytbf_fetchtoken(mytbf_t *, int);

int mytbf_returntoken(mytbf_t *, int);

int mytbf_destroy(mytbf_t *);

int mytbf_checktoken(mytbf_t *);
#endif // !MYTBF_H__
