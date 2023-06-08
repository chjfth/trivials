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

void doWork(unsigned int ui)
{
	CSpam sobj;
	char bufa[8] = "AAAAAAA";
	
	will_throw(0xBBBBbbbb);
}

int main(int argc, char *argv[])
{
	printf("Start excptest3 ...\n");
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
//	cl /EHsc /MT /Zi /JMC- /GS- /FAsc excptest3.cpp worklib.cpp
//
// excptest3 Memo:
//	Compared to excptest2, will_throw() is now declared 'noexcept',
//	then, doWork() now does NOT have EH-push/pop code.
//	That is, compiler thinks doWork() does NOT need to equipped with
//	EH-push/pop code.
//
//	doWork()'s epilog do NOT hold-stop stack-unwinding for any Exception
//	thrown from inside doWork(). This is the case no matter will_throw()
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
In will_throw()...
[Caught!] e.what() is: The will_throw() really throws.
Safe return from will_throw().
*/
// We see that, although stack unwinding has out-reached to main(),
// but doWork()'s sobj's dtor is NOT executed.
// 
// VC2019 16.11 and gcc-7.5 both exhibit such behavior.