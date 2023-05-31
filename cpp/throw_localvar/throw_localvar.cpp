#include <stdio.h>
#include <stdexcept>

void do_throw()
{
	std::exception local_excobj("Boom!");
	printf("do_throw()'s localvar   @[%p]\n", &local_excobj);

	throw(local_excobj);
}

int main(int argc, char *argv[])
{
	const char* ptrexe = argv[0];
	printf("main's localvar         @[%p]\n", &ptrexe);

	try	{
		do_throw();
	}
	catch (std::exception &rcv_excobj) {
		printf("[Caught] Got rcv_excobj @[%p]\n", &rcv_excobj);
		printf("         excobj message: %s\n", rcv_excobj.what());
	}
}

