#include <stdio.h>
#include <stdexcept>

extern void will_throw(unsigned int) noexcept;

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

void doWork(unsigned int ui) noexcept // *new*
{
	CSpam sobj;
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
//	cl /EHsc /MT /Zi /JMC- /GS- /FAsc excptest4.cpp worklib.cpp
//
// excptest4 Memo:
//	Compared to excptest3, doWork() is now defined with 'noexcept',
//	so, doWork() will hold-stop Exception's propagation and calls.
//	std::terminate() to quit the whole program.
//	That is, we will NOT see [Caught!] on program run.
