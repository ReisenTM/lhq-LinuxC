#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#define FILENAME "/tmp/out"
static int Deamon_init(){
	int fd;
	pid_t pid;
	fd = open("/dev/null",O_RDONLY);
	dup2(fd,0);
	dup2(fd,1);
	dup2(fd,2);
	if(fd>3)	close(fd);
	pid=fork();
	if(pid<0) return -1;
	else if(pid>0) exit(0);
	setsid();
	chdir("/");
	return 0;
	

}
int main(){
	int i=1;
	FILE* fl=NULL;
	fl=fopen(FILENAME,"w");
	if(fl==NULL){
		perror("fopen");
		exit(1);
	}
	if(Deamon_init()){
		fprintf(stderr,"err\n");
		exit(1);
	}
	while(i++){
		fprintf(fl,"%d\n",i);
		fflush(fl);
		sleep(1);
	}
	exit(0);
}
