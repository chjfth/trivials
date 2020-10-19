#include <stdio.h>

#ifndef MYEXPORT
#define MYEXPORT
#endif

MYEXPORT
void FooX(int i)
{
	printf("foo2.c: fooX(%d)\n", i);
	i++;
//	foo3(i);
}
