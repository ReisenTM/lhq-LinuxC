#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
int main(int argc,char*argv[]){
    puts("Begin!");
    pid_t pid;
    printf("current process pid: %d\n",getpid());
    fflush(NULL);
    pid=fork();
    if(pid<0){
        perror("fork");
        exit(1);
    }
    if(pid==0){//child process
        puts("child process begin!");
        execl("/usr/bin/date","date",NULL);//NULL mean the end of argv
        puts("child end");
    }
    wait(NULL);
    puts("end");
    return 0;
}
