#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	alarm(5);
	while(1);
	exit(0);
}
