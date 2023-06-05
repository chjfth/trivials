#include <stdio.h>
#include <stdexcept>

void will_throw()
{
	printf("In will_throw()...\n");

	throw std::runtime_error("The will_throw() really throws.");
}
