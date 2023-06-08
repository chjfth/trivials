#include <stdio.h>
#include <stdexcept>

void will_throw(unsigned int ui)
{
	char bufb[8] = "BBBBBBB";
	printf("In will_throw()...\n");

	throw std::runtime_error("The will_throw() really throws.");
}
