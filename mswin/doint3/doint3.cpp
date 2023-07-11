#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

void catch3()
{
	__try 
	{
		// Note: If run with VC2010/VC2019 debugger attached,
		// the __except {...} block will NOT be executed.
		// That's a deliberate design of VSIDE.
		//
		// In order to see the __except {...} block executed,
		// we need to run it under WinDBG(10.0.18362), and 
		// apply "Go Unhandled Exception"(gn) .
#ifdef _M_IX86
		__asm int 3;
#else
		DebugBreak();
#endif
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		printf("Caught my `int 3` !\n");
	}
}

void raise3()
{
#ifdef _M_IX86
	__asm int 3;
#else
	DebugBreak();
#endif
}

int main(int argc, char* argv[])
{
	if(argc==1)
	{
		printf("doint3 raise3() ....\n");
		raise3();
	}
	else
	{
		printf("doint3 catch3().\n");
		catch3();
	}

	printf("doint3 finishes.\n");
	return 0;
}

