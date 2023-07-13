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

	myfunc1(1);
	printf("\n");
	Sleep(500);

	printf("== Try some other exception ==\n");
	int ret = 0;
	__try {
		ret = my_do_div(3, 0);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
		printf("[Caught] division by zero.\n");
	}
	printf("\n");

	myfunc1(2);
	printf("\n");
	Sleep(500);

	Untrace_UserFunc(myfunc1);
}

int main()
{
	test_tracing_myfunc();

	myfunc1(3);

	return 0;
}
