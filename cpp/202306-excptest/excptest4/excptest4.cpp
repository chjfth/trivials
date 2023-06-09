#include <stdio.h>
#include <stdexcept>

extern void mybar(unsigned int) noexcept;

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

void myfoo(unsigned int ui) noexcept // *new*
{
	CSpam sobj;
	char bufa[8] = "FFFFFFF";
	
	mybar(0xB0B0B0B0);
}

int main(int argc, char *argv[])
{
	printf("Start excptest4 ...\n");
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
//	cl /EHsc /MT /Zi /JMC- /GS- /FAsc excptest4.cpp mybar.cpp
//
// excptest4 Memo:
//	Compared to excptest3, myfoo() is now defined with 'noexcept',
//	so, myfoo() will hold-stop Exception's propagation and calls.
//	std::terminate() to quit the whole program.
//	That is, we will NOT see [Caught!] on program run.
