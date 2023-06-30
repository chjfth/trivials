#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

enum HowToExcept_et
{
	NoExcept = 0,
	DivideByZero = 1, // "div0"
	ReadNullptr = 2,  // "readnull"
	CxxThrow = 3,     // "throw"
};

//bool g_read_nullptr = false; // false: will do divide-by-zero
HowToExcept_et g_howexcp = NoExcept;

class C1
{
public:
	C1()  { printf("C1-ctor()\n"); m1=0x33333333; }
	~C1() { printf("C1-dtor()\n"); }
	int m1;
};

int do_bad()
{
	HowToExcept_et how = g_howexcp;
	int ret = 0;
	int divisor = 0;
	
	if(how==DivideByZero)
		ret = 3 / divisor;
	else if(how==ReadNullptr)
		ret = ((int*)nullptr)[0];
	else if(how==CxxThrow)
		throw 3;

	return ret;
}

int cxx_raii()
{
	C1 c1obj;
	int ret = do_bad();
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
		// Only catch DivideByZero
	{
		printf("[Caught] in test_seh_guard __except{...}\n");
	}
}

int main(int argc, char* argv[])
{
	if(argc>1)
	{
		if(_stricmp(argv[1], "div0")==0) {
			g_howexcp = DivideByZero;
			printf("Will do DivideByZero.\n");
		}
		else if(_stricmp(argv[1], "readnull")==0) {
			g_howexcp = ReadNullptr;
			printf("Will do ReadNullptr.\n");
		}
		if(_stricmp(argv[1], "throw")==0) {
			g_howexcp = CxxThrow;
			printf("Will do C++ throw.\n");
		}
	}

	if(g_howexcp==NoExcept)
		printf("Will not raise any exception.\n");
	
	test_seh_guard();

	return 0;
}

