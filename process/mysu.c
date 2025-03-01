#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


int main(int argc,char** argv){
	if(argc<3){
		fprintf(stderr,"Usage...\n");
		exit(1);
	}
	pid_t pid;
	pid=fork();
	if(pid<0){
		perror("fork");
		exit(1);
	}
	if(pid==0){
		if(setuid(atoi(argv[1]))<0){
			perror("setuid:");
			exit(1);
		}
		execvp(argv[2],argv+2);
		perror("exec:");
		exit(1);
	}else{
		wait(NULL);
	}
	exit(0);
}
