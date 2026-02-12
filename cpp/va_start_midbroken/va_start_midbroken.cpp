#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define Arg1 0x10
#define Arg2 0xA11112222
#define Arg3 0x30
#define Arg4 0xB12345678

__int64 vargs[5] = { 0, Arg1, Arg2, Arg3, Arg4 };

void test_midbroken(const char *fmt, 
	__int64 varg1, __int64 varg2,
	...)
{
	va_list args;
	va_start(args, fmt);
	// -- Gotcha! This statement causes subtle runtime error on x64 Release-build EXE.
	//
	// If we compile with /Od (Debug build), varg1 and varg2 will be pushed 
	// to the stack and va_arg() always fetches correct values from the caller.
	//
	// But if we use /O2 optimization(Release build), varg1 and varg2's value are 
	// only stored in RDX and R8 registers, NOT pushed to the stack-frame. 
	// So first two va_arg() fetching will fetch random values from the stack-frame.

	for (int i = 1; i <=4 ; i++)
	{
		__int64 param = va_arg(args, __int64);
		if (param == vargs[i])
			printf("Arg%d OK. 0x%llX\n", i, param);
		else
			printf("Arg%d broken! 0x%llX -> 0x%llX\n", i, vargs[i], param);
	}

	va_end(args);
}

int main()
{
#ifdef _DEBUG
	const char *szDebug = "DEBUG";
#else
	const char *szDebug = "non-DEBUG";
#endif

	printf("Env-trait: %d bits, %s\n", (int)(sizeof(void*)*8), szDebug);

	test_midbroken("fmtstring", Arg1, Arg2, Arg3, Arg4);

	return 0;
}
