#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
//void testFun(void){
//	printf("test\n");
	
//}
int main(){
	long long count=0;
//	atexit(testFun);
	time_t timeEnd=time(NULL)+5;
	while(time(NULL)!=timeEnd){
		count++;
	}
	printf("count:%lld\n",count);	
	exit(0);
}
