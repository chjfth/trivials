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

void doWork(unsigned int ui) noexcept
{
//	CSpam sobj; // *new* no local C++ object to destruct
	char bufa[8] = "AAAAAAA";
	
	will_throw(0xBBBBbbbb);
}

int main(int argc, char *argv[])
{
	printf("Start excptest5 ...\n");
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
//	cl /EHsc /MT /Zi /JMC- /GS- /FAsc excptest5.cpp worklib.cpp
//
// excptest5 Memo:
//	Compared to excptest4, this time, doWork() does NOT define a local
//	C++ object, so doWork() does not need to destruct any object on
//	Exception-triggered stack-unwind.
//	What's more, we tell the compiler will_throw() is 'noexcept',
//	then the compiler decides(even doWork() is marked 'noexcept'):
//	no EH-push/pop instructions are required.
//
//	So the result is: On will_throw()'s actually throwing an Exception,
//	NO std::terminate() sentinel is there to hold-stop the program.
//	We see program output like this:
/*
Start try{} ...
In will_throw()...
[Caught!] e.what() is: The will_throw() really throws.
Safe return from will_throw().
 */

// BUT: gcc-7.5/gcc-12 will STILL hold-stop the Exception at doWork().
// We will not see [Caught!] from a gcc-compiled excptest5 .
