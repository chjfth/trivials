#include <stdio.h>
#include <stdexcept>

extern void mybar(unsigned int);

class CSpam
{
public:
	CSpam() : m1(0x53535353) {
		printf("Spam ctor for [@%p]\n", this);
	}
	~CSpam() {
		printf("Spam dtor for [@%p]\n", this);
	}
	int m1;
};

void myfoo(unsigned int ui)
{
	CSpam sobj; // *new* in excptest2.cpp
	char bufa[8] = "FFFFFFF";
	
	mybar(0xB0B0B0B0);
}

int main(int argc, char *argv[])
{
	printf("Start excptest2 ...\n");
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
//	cl /EHsc /MT /Zi /JMC- /GS- /FAsc excptest2.cpp worklib.cpp
//
// excptest2 Memo:
//	myfoo() needs to destruct C++ object sobj on myfoo's quit,
//	so myfoo() needs EH-push/pop code.
