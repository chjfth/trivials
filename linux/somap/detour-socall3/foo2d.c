#include <stdio.h>

#ifndef MYEXPORT
#define MYEXPORT
#endif

__attribute__((visibility("default"))) void FooX(int i);

void Foo2(int i)
{
	printf("Via Foo2(%d), FooX declare default, define hidden\n", i);
	FooX(i);
}
