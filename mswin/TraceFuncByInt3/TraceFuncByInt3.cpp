// Important! See program comments at start of TraceAPI.cpp .

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

#include "TraceAPI.h"

int my_do_div(int d1, int d2)
{
	int ret = d1 / d2;
	return ret;
}

__declspec(noinline) // Ensure caller "jumps to" this function body, in Release-build.
void myfunc1(int n)
{
	printf("myfunc1(%d) really executing ...\n", n);
}

void test_tracing_myfunc()
{
	Trace_UserFunc(myfunc1);

	myfunc1(2);
	printf("\n");
	Sleep(500); // no importance

	printf("== Try some other exception ==\n");
	int ret = 0;
	__try {
		ret = my_do_div(3, 0);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("[Caught] division by zero.\n");
	}
	printf("\n");

	myfunc1(3);  // no importance
	printf("\n");
	Sleep(500);

	Untrace_UserFunc(myfunc1);
}

int main()
{
	myfunc1(1);
	printf("\n");

	test_tracing_myfunc();

	myfunc1(4);

	return 0;
}

/* [2023-07-13] Running output is sth like this:

C:\>D:\gitw\trivials\mswin\TraceFuncByInt3\bin-v100\Win32\Debug\TraceFuncByInt3.exe
myfunc1(1) really executing ...

[Matt] In int3_BreakpointHandler: ExcpCode=0x80000003 EIP=00401020
[Matt] Do STATUS_BREAKPOINT handling.
[Matt] ## Monitored function is being called.
[Matt] In int3_BreakpointHandler: ExcpCode=0x80000004 EIP=00401021
[Matt] Do STATUS_SINGLE_STEP handling.
myfunc1(2) really executing ...

== Try some other exception ==
[Matt] In int3_BreakpointHandler: ExcpCode=0xC0000094 EIP=0040100F
[Caught] division by zero.

[Matt] In int3_BreakpointHandler: ExcpCode=0x80000003 EIP=00401020
[Matt] Do STATUS_BREAKPOINT handling.
[Matt] ## Monitored function is being called.
[Matt] In int3_BreakpointHandler: ExcpCode=0x80000004 EIP=00401021
[Matt] Do STATUS_SINGLE_STEP handling.
myfunc1(3) really executing ...

myfunc1(4) really executing ...

*/