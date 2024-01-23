#include <stdio.h>

#ifndef MYEXPORT
#define MYEXPORT
#endif

extern void FooX(int);

MYEXPORT
void use_bar(int i)
{
	printf("In use_bar.c(%d)\n", i);
	FooX(i);
}
