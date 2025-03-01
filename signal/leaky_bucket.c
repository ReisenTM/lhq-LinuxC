#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include  <signal.h>
#define BUFSIZE 10

static volatile int8_t flag=0;
static void Alrm_Handler(int s){
	flag=0;//重置标记
	alarm(1);
}
int main(int argc,char**argv){
	if(argc<2){
		fprintf(stderr,"Usage...\n");
		exit(1);
	}
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
	int pos=0,res,len;
	char buf[BUFSIZE];

	signal(SIGALRM,Alrm_Handler);
	alarm(1);
	flag=1;
	while(1){
		
		while(flag){
			//puts("test!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
			pause();
		}
		flag=1;
		len=read(fds,buf,BUFSIZE);
		if(len<0){	
			if(errno==EINTR)
				continue;
			perror("read");
			break;
		}
		if(len==0)
			break;
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
	exit(0);
}
