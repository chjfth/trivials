#include <stdio.h>

#ifndef MYEXPORT
#define MYEXPORT
#endif

#include "local-foox.h"
	
MYEXPORT
void Foo2(int i)
{
	printf("Via Foo2(%d) will call local\n", i);
	FooX(i);
}
