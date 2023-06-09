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

void myfoo(unsigned int ui) noexcept
{
//	CSpam sobj; // *new* no local C++ object to destruct
	char bufa[8] = "FFFFFFF";
	
	mybar(0xB0B0B0B0);
}

int main(int argc, char *argv[])
{
	printf("Start excptest5 ...\n");
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
//	cl /EHsc /MT /Zi /JMC- /GS- /FAsc excptest5.cpp mybar.cpp
//
// excptest5 Memo:
//	Compared to excptest4, this time, myfoo() does NOT define a local
//	C++ object, so myfoo() does not need to destruct any object on
//	Exception-triggered stack-unwind.
//	What's more, we tell the compiler mybar() is 'noexcept',
//	then the compiler decides(even myfoo() is marked 'noexcept'):
//	no EH-push/pop instructions are required.
//
//	So the result is: On mybar()'s actually throwing an Exception,
//	NO std::terminate() sentinel is there to hold-stop the program.
//	We see program output like this:
/*
Start excptest5 ...
In mybar()...
[Caught!] e.what() is: mybar() really throws.
Safe return from mybar().
 */

// BUT: gcc-7.5/gcc-12 will STILL hold-stop the Exception at myfoo().
// We will not see [Caught!] from a gcc-compiled excptest5 .
