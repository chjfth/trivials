#undef NDEBUG // always enable assert()
#include <stdio.h>
#include <assert.h>

void do_test()
{
	printf("Before assert(0)\n");

	assert(0);

	printf("After assert(0)\n");
}

int main()
{
	do_test();
	
	return 0;
}
