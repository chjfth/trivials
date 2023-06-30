#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

bool g_read_nullptr = false; // false: will do divide-by-zero

class C1
{
public:
	C1()  { printf("C1-ctor()\n"); m1=0x33333333; }
	~C1() { printf("C1-dtor()\n"); }
	int m1;
};

int do_bad(int divisor)
{
	int ret = 0;
	if(!g_read_nullptr)
		ret = 3 / divisor;
	else
		ret = ((int*)nullptr)[0];

	return ret;
}

int cxx_raii()
{
	C1 c1obj;
	int ret = do_bad(0);
	return ret;
}

typedef int ExcpFilterDecision_t;

ExcpFilterDecision_t Filter_RecoverDvZero(DWORD excpcode)
{
	printf("Filter: See exception-code=0x%X\n", excpcode);

	if(excpcode==EXCEPTION_INT_DIVIDE_BY_ZERO)
		return EXCEPTION_EXECUTE_HANDLER;
	else
		return EXCEPTION_CONTINUE_SEARCH;
}

void test_seh_guard()
{
	__try
	{
		cxx_raii();
	}
	__except(Filter_RecoverDvZero(GetExceptionCode()))
	{
		printf("[Caught] in test_seh_guard __except{...}\n");
	}
}

int main(int argc, char* argv[])
{
	if(argc>1)
		g_read_nullptr = true;
	
	test_seh_guard();

	return 0;
}

