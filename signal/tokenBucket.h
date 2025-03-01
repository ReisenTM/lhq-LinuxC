#ifndef MYTB_H__
#define MYTB_H__
#define MAXSIZE 1024
typedef void mytb_t;
mytb_t* Mytb_Init(int cps,int burst);
int Mytb_Fetchtoken(mytb_t* tb,int n);
int Mytb_Returntoken(mytb_t* tb,int n);
int Mytb_Destroy(mytb_t* tb);
#endif
