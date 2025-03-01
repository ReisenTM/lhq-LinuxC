#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
void INT_Handler(int n){
	write(1,"!",1);
}
int main(){
	int i=0;
	signal(2,INT_Handler);
	do{
		if(i>=10) break;
		write(1,"*",1);
		sleep(1);
	}while(++i);
	putchar('\n');
	puts("end");
	exit(0);
}
