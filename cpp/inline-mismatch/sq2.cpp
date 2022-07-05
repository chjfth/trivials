#include <stdio.h>

#ifdef REMOVE_INLINE
#define __inline
#endif

extern"C" __inline int get_square(int n)
{
	return n*n + 2;
}

#ifndef REMOVE_CALLER
extern"C" void prn_square2(int n)
{
	int r = get_square(n);
	printf("prn-square2(%d) = %d\n", n, r);
}
#endif
