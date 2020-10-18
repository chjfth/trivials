#include <stdio.h>

__attribute__((visibility("default")))
void FooX(int i)
{
	printf("local-foox.c: fooX(%d)\n", i);
}
