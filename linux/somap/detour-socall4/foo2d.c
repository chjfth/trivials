#include <stdio.h>

#ifndef MYEXPORT
#define MYEXPORT
#endif

__attribute__((visibility("hidden"))) void FooX(int i);

void Foo2(int i)
{
	printf("Via Foo2(%d), FooX declare hidden, define default\n", i);
	FooX(i);
}
