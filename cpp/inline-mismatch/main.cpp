#include <stdio.h>
#include <stdlib.h>

extern"C" int get_square(int n);
extern"C" void prn_square1(int n);
extern"C" void prn_square2(int n);

int main(int argc, char *argv[])
{
	int n = 0;
	if(argc>1)
		n = atoi(argv[1]);
	
#ifndef REMOVE_CALLER
	prn_square1(n);
	prn_square2(n);
#endif
	
	int r = get_square(n);
	printf("main() calling external get_square(%d)= %d\n", n, r);
	
	return 0;
}
