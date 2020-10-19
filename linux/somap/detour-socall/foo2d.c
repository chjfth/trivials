#include <stdio.h>

#ifndef MYEXPORT
#define MYEXPORT
#endif

MYEXPORT
void FooX(int i)
{
	printf("foo2d.c: fooX(%d)\n", i);
	i++;
}

MYEXPORT
void Foo2(int i)
{
	printf("Via Foo2(%d)\n", i);
	FooX(i);
}
