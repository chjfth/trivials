#include <stdio.h>
#include <stdlib.h>

void use_foo2(int i);
void use_bar(int i);


int main(int argc, char **argv)
{
	int i = 0;
	if(argc>1)
		i = atoi(argv[1]);
	printf("vmaind(%d)\n", i);
	i++;
	
	use_foo2(i);
	use_bar(i);
	
	return 0;
}
