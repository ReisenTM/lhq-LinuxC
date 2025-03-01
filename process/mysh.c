#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glob.h>
#include <sys/wait.h>
#include <unistd.h>
#define DELIM " \t\n" 
typedef struct{
	glob_t globres;
}cmd_t;
void Mysh_ShowPrompt(){
	printf("reisen@mysh:$");
}
static void Mysh_ParseStr(char* str,cmd_t* cmd){
	char *token=NULL;
	int i=0;
	while(1){
		token=strsep(&str,DELIM);
		if(token==NULL){
			break;
		}
		if(token[0]=='\0'){
			continue;
		}
		
		glob(token,GLOB_NOCHECK|GLOB_APPEND*i,NULL,&cmd->globres);//the reason why we use "i"is that we dont make sure what we applend is in the first apppend
		i=1;
		
	}
}
void Mysh_ExcuteExternalCmd(cmd_t * cmd){
	pid_t pid;
	pid=fork();
	if(pid<0){//error happens
		perror("fork()");
		return;
	}else if(pid ==0){//child process
		execvp(cmd->globres.gl_pathv[0],cmd->globres.gl_pathv);
		//if not be replaced ,which means error occurs
		perror("exec");
		exit(1);
	}else{//father process
		wait(NULL);
	}
}
void Mysh_ExcuteInternalCmd(cmd_t *cmd){
	/**/
}
int main(int argc,char* argv[]){
    char *StrBuf=NULL;
    size_t Buf_Size=0;
    //glob_t globres;   //for a long term proj we should make a struct to deal with any res it return
    cmd_t cmd;
    while(1){
        Mysh_ShowPrompt();

	//get command string
        if(getline(&StrBuf,&Buf_Size,stdin)<0){
		perror("getline()");
		exit(1);
	}
	//parse command
	Mysh_ParseStr(StrBuf,&cmd);
	//excute command
	Mysh_ExcuteExternalCmd(&cmd);
    }	


    exit(0);
}
