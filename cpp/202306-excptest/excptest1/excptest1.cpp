#include <stdio.h>
#include <stdexcept>

extern void mybar(unsigned int);

void myfoo(unsigned int ui)
{
	char bufa[8] = "FFFFFFF";
	
	mybar(0xB0B0B0B0);
}

int main(int argc, char *argv[])
{
	printf("Start excptest1 ...\n");
	try
	{
		myfoo(0xF0F0F0F0);
	}
	catch (std::exception &e)
	{
		printf("[Caught!] e.what() is: %s\n", e.what());
	}
	
	printf("Safe return from mybar().\n");
	
	return 0;
}

// VC2019 compile and link command
//	cl /EHsc /MT /Zi /JMC- /GS- /FAsc excptest1.cpp mybar.cpp
