#include <stdio.h>

__attribute__((visibility("hidden")))
void FooX(int i)
{
	printf("local-foox.c: fooX(%d)\n", i);
}
