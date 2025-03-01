#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "tokenBucket.h"
#define CPS 10
#define BURST 100
#define BUFSIZE 1024
int main(int argc,char**argv){
	if(argc<2){
		//如果输入不合法，报错退出
		fprintf(stderr,"Usage...\n");
		exit(1);
	}
	//令牌桶
	int fds,fdd=1;
	//反复读取直至成功
	do{
		fds=open(argv[1],O_RDONLY);
		if(fds<0){
			if(errno!=EINTR){
				perror("open");
				exit(1);
			}
		}
	}while(fds<0);
	int pos,res,len,size;
	char buf[BUFSIZE];
	mytb_t* mtb=Mytb_Init(CPS,BURST);
	while(1){
		size=Mytb_Fetchtoken(mtb,BUFSIZE);
		
		while((len=read(fds,buf,size))<0)
		{
			if(errno==EINTR)
				continue;
			perror("read");
			break;
		}
		if(len==0)
			break;
		if(size-len>0)
			Mytb_Returntoken(mtb,size-len);

		pos=0;
		while(len>0){
			res=write(fdd,buf+pos,len);
			if(res<0){
				if(errno==EINTR)
					continue;
				perror("write");
				exit(1);
			}
			pos+=res;
			len-=res;
		}
	}
	close(fds);
	Mytb_Destroy(mtb);
	exit(0);
}
