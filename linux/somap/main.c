#include <stdio.h>
#include <stdlib.h>

extern void Foo1(int);

int main(int argc, char **argv)
{
	int i = 0;
	if(argc>1)
		i = atoi(argv[1]);

	printf("main(%d)\n", i);
	i++;
	Foo1(i);
	return 0;
}
