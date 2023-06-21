#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

class C1 {
public:
	C1() { printf("C1 ctor() @[%p]\n", this); }
	~C1(){ printf("C1 dtor() @[%p]\n", this); }
	int m1;
};

int main(int argc, char* argv[])
{
	C1 cobj;
	__try 
	{
		if(argc==1)
		{
			printf("In __try block, now will do an invalid memory access.\n");
			*(int*)nullptr = 5;  // This causes an access violation
		}
		else
		{
			printf("In __try block, just a normal printf.\n");
		}
	}
	__finally {
		const char *psz = AbnormalTermination() 
			? "MY Abnormal termination !!" 
			: "my Normal termination.";

		printf("In __finally block: %s\n", psz);
	}

	printf("After __finally block, normal process termination.\n");	
	return 0;
}

/* Program memo: for VC2010+, 

  * if compile with /EHsc or /EHa, cl.exe spouts error:

	error C2712: Cannot use __try in functions that require object unwinding

  * if compile without any /EH options, compile and link will success, 
    but cobj's dtor will not be executed if Win32 exception is thrown.
*/
