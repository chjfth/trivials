#include <stdio.h>
#include <stdexcept>

void mybar(unsigned int ui)
{
	char bufb[8] = "BBBBBBB";
	printf("In mybar()...\n");

	throw std::runtime_error("mybar() really throws.");
}
