#include <stdio.h>
#include <stdexcept>

void mybar(unsigned int ui)
{
	char bufb[8] = "BBBBBBB";
	printf("In mybar(), bufb=@%p...\n", bufb);

	void* pframe0 = __builtin_frame_address(0);
	void* pret0 = __builtin_return_address(0);
	printf("pframe0 = [@%p] , pret0 = [@%p]\n", pframe0, pret0);
	(void)pframe0; (void)pret0;

	void* pframe1 = __builtin_frame_address(1);
	void* pret1 = __builtin_return_address(1);
	printf("pframe1 = [@%p] , pret1 = [@%p]\n", pframe1, pret1);
	(void)pframe1; (void)pret1;

	void* pframe2 = __builtin_frame_address(2);
	void* pret2 = __builtin_return_address(2);
	printf("pframe2 = [@%p] , pret2 = [@%p]\n", pframe2, pret2);
	(void)pframe2; (void)pret2;

	throw std::runtime_error("mybar() really throws.");
}
