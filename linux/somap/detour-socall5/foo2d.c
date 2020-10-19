#include <stdio.h>

#ifndef MYEXPORT
#define MYEXPORT
#endif

void Foo2(int i)
{
	printf("Via Foo2(%d) link with -Bsymbolic\n", i);
	FooX(i);
}
