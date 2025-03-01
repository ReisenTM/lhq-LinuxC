#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(){
	int i=0;
	do{
		if(i>=10) break;
		write(1,"*",1);
		sleep(1);
	}while(++i);
	putchar('\n');
	puts("end");
	exit(0);
}
