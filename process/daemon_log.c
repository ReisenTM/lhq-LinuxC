#include <fcntl.h>
#include <syslog.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#define FILENAME "/tmp/out"
static int Deamon_init(){
	int fd;
	pid_t pid;
	fd = open("/dev/null",O_RDONLY);
	if(fd<0){
		syslog(LOG_INFO,"open()");
		return -1;
	}
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
	openlog("Deamon",LOG_PID,LOG_DAEMON);
	if(fl==NULL){
		perror("fopen");
		exit(1);
	}
	if(Deamon_init()){
		syslog(LOG_ERR,"Deamon init faild");
exit(1);
	}
	syslog(LOG_INFO,"Deamon init success");
	while(i++){
		fprintf(fl,"%d\n",i);
		fflush(fl);
		syslog(LOG_INFO,"%d",i);
		sleep(1);
	}
	fclose(fl);
	closelog();
	exit(0);
}
