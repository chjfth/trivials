#include <stdio.h>
#include <stdexcept>

void mybar(unsigned int u1, unsigned int u2)
{
	unsigned int zre = u1 / u2;
	printf("Divided-by-zero result: %u\n", zre);
}

void myfoo(unsigned int ui)
{
	char bufa[8] = "FFFFFFF";
	
	mybar(0xB0B0B0B0, 0);
}

int main(int argc, char *argv[])
{
	printf("Start try {} ...\n");
	try
	{
		myfoo(0xF0F0F0F0);
	}
	catch (...)
	{
		printf("[Caught Divided-by-zero!]\n");
	}
	
	printf("Safe return from mybar().\n");
	
	return 0;
}


// Memo:
// If compile with VC2019 `cl /EHa excp-dvzero.cpp`, the catch(...) block can
// catch SEH-exception, like Divide-by-zero case. 
// But it is not portable, i.e. not supported by Linux gcc.
