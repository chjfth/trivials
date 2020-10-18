#include <stdio.h>

#ifndef MYEXPORT
#define MYEXPORT
#endif

MYEXPORT
void use_bar(int i)
{
	printf("In use_bar.c(%d)\n", i);
	FooX(i);
}
