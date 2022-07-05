#include <stdio.h>

#ifdef REMOVE_INLINE
#define __inline
#endif

extern"C" __inline int get_square(int n)
{
	return n*n + 1;
}

#ifndef REMOVE_CALLER
extern"C" void prn_square1(int n)
{
	int r = get_square(n);
	printf("prn-square1(%d) = %d\n", n, r);
}
#endif

#ifdef DULL_CALLER
void dull_func()
{
	get_square(0);
}
#endif
