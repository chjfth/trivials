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

void myfoo(unsigned int ui)
{
	CSpam sobj;
	char bufa[8] = "FFFFFFF";
	
	mybar(0xB0B0B0B0);
}

int main(int argc, char *argv[])
{
	printf("Start excptest3 ...\n");
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
//	cl /EHsc /MT /Zi /JMC- /GS- /FAsc excptest3.cpp worklib.cpp
//
// excptest3 Memo:
//	Compared to excptest2, mybar() is now declared 'noexcept',
//	then, myfoo() now does NOT have EH-push/pop code.
//	That is, compiler thinks myfoo() does NOT need to equipped with
//	EH-push/pop code.
//
//	myfoo()'s epilog do NOT hold-stop stack-unwinding for any Exception
//	thrown from inside myfoo(). This is the case no matter mybar()
//	is *declared* 'noexcept' or not.
//	.
//	So, the std::runtime_error Exception will propagate to main()'s catch(),
//	and we see [Caught!] on program run.
//
// [2023-06-08] And there also arise a subtle problem, dtor-leaking.
/*
> excptest3
Start try{} ...
Spam ctor for [@008FFEE8]
In mybar()...
[Caught!] e.what() is: mybar() really throws.
Safe return from mybar().
*/
// We see that, although stack unwinding has out-reached to main(),
// but myfoo()'s sobj's dtor is NOT executed.
// 
// VC2019 16.11 and gcc-7.5 both exhibit such behavior.