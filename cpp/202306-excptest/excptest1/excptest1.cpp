#include <stdio.h>
#include <stdexcept>

extern void will_throw(unsigned int);

void doWork(unsigned int ui)
{
	char bufa[8] = "AAAAAAA";
	will_throw(0xBBBBbbbb);
}

int main(int argc, char *argv[])
{
	bool path1 = (argc > 1 && argv[1][0] == '1') ? true : false;
	
	printf("Start try{} ...\n");
	try
	{
		doWork(0xAAAAaaaa);
	}
	catch (std::exception &e)
	{
		printf("[Caught!] e.what() is: %s\n", e.what());
	}
	
	printf("Safe return from will_throw().\n");
	
	return 0;
}

// VC2019 compile and link command
//	cl /EHsc /MT /Zi /JMC- /GS- /FAsc excptest1.cpp worklib.cpp
