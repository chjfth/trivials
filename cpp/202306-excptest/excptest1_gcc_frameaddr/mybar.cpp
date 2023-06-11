#include <stdio.h>
#include <stdexcept>

void mybar(unsigned int ui)
{
	char bufb[8] = "BBBBBBB";
	printf("In mybar(), bufb=@%p...\n", bufb);

	void* pframe0 = __builtin_frame_address(0);
	printf("pframe0 = [@%p]\n", pframe0);
	(void)pframe0;

	void* pframe1 = __builtin_frame_address(1);
	printf("pframe1 = [@%p]\n", pframe1);
	(void)pframe1;

	void* pframe2 = __builtin_frame_address(2);
	printf("pframe2 = [@%p]\n", pframe2);
	(void)pframe2;

	throw std::runtime_error("mybar() really throws.");
}
