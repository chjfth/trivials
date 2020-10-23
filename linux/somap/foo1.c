#include <stdio.h>
void Foo1(int i)
{
	printf("foo1(%d)\n", i);
	i++;
	FooX(i);
}
