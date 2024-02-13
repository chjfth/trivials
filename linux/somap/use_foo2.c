#include <stdio.h>

#ifndef MYEXPORT
#define MYEXPORT
#endif

extern void FooX(int);

MYEXPORT
void use_foo2(int i)
{
	printf("In use_foo2.c(%d)\n", i);
	FooX(i);
}
