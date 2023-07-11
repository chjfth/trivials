#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// https://learn.microsoft.com/en-us/windows/win32/debug/using-a-vectored-exception-handler

// NOTE: The CONTEXT structure contains processor-specific 
// information. This sample was designed for X86 processors.

//addVectoredExceptionHandler constants:
//CALL_FIRST means call this exception handler first;
//CALL_LAST means call this exception handler last
#define CALL_FIRST 1  
#define CALL_LAST 0

LONG Sequence = 1;
LONG Actual[3];

LONG WINAPI
VectoredHandler1(EXCEPTION_POINTERS* ExceptionInfo)
{
    UNREFERENCED_PARAMETER(ExceptionInfo);

    Actual[0] = Sequence++;
    return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI
VectoredHandler2(EXCEPTION_POINTERS* ExceptionInfo)
{
    UNREFERENCED_PARAMETER(ExceptionInfo);

    Actual[1] = Sequence++;
    return EXCEPTION_CONTINUE_SEARCH;
}

LONG WINAPI
VectoredHandler3(EXCEPTION_POINTERS* ExceptionInfo)
{
    UNREFERENCED_PARAMETER(ExceptionInfo);

    Actual[2] = Sequence++;
    return EXCEPTION_CONTINUE_SEARCH;
}

static void Eip_inc(PCONTEXT Context)
{
#ifdef _AMD64_
    Context->Rip++;
#else
    Context->Eip++;
#endif    
}

LONG WINAPI
VectoredHandlerSkip1(EXCEPTION_POINTERS* ExceptionInfo)
{
    PCONTEXT Context;

    Sequence++;
    Context = ExceptionInfo->ContextRecord;
    Actual[0] = 0xcc;

    Eip_inc(Context);
	
    return EXCEPTION_CONTINUE_EXECUTION;
}

LONG WINAPI
VectoredHandlerSkip2(EXCEPTION_POINTERS* ExceptionInfo)
{
    PCONTEXT Context;

    Sequence++;
    Context = ExceptionInfo->ContextRecord;
    Actual[1] = 0xcc;

    Eip_inc(Context);

	return EXCEPTION_CONTINUE_EXECUTION;
}

LONG WINAPI
VectoredHandlerSkip3(EXCEPTION_POINTERS* ExceptionInfo)
{
    PCONTEXT Context;

    Sequence++;
    Context = ExceptionInfo->ContextRecord;
    Actual[2] = 0xcc;

    Eip_inc(Context);

	return EXCEPTION_CONTINUE_EXECUTION;
}

BOOL assert_match(char* Variation, PLONG e, PLONG a)
{
    int i;
    BOOL Pass = TRUE;

    for (i = 0; i < 3; i++)
    {
        if (e[i] != a[i])
        {
            if (Variation)
            {
                printf("%s Failed at %d : Expected %d vs Actual %d\n",
                    Variation, i, e[i], a[i]);
            }
            Pass = FALSE;
        }

        // Clear actual for next pass.
        a[i] = 0;
    }

    // Get ready for next pass.
    Sequence = 1;

    if (Variation)
    {
        printf("Variation %s : %s\n", Variation, (Pass ? "Passed" : "Failed"));
    }
    return Pass;
}

void check_all_zeros()
{
    LONG e[3];
    BOOL b = 0;

    e[0] = 0; e[1] = 0; e[2] = 0;
    __try
    {
        RaiseException(0x40004444, 0, 0, NULL);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        b = assert_match(NULL, e, Actual);
    }
    if (!b)
    {
        printf("Fatal error, handlers still registered.\n");
    }
}

void IllegalInst()
{
    char* ptr = 0;
    *ptr = 0;
}

void Test1()
{
    PVOID h1, h2, h3;
    LONG e[3];

    e[0] = 1; e[1] = 2; e[2] = 3;
    h2 = AddVectoredExceptionHandler(CALL_FIRST, VectoredHandler2);
    h3 = AddVectoredExceptionHandler(CALL_LAST, VectoredHandler3);
    h1 = AddVectoredExceptionHandler(CALL_FIRST, VectoredHandler1);

    __try
    {
        RaiseException(0x40004444, 0, 0, NULL);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        assert_match("Test1a", e, Actual);
    }

    RemoveVectoredExceptionHandler(h1);
    RemoveVectoredExceptionHandler(h3);
    RemoveVectoredExceptionHandler(h2);
    check_all_zeros();

	printf("\n");
}

void Test2()
{
    PVOID h1, h2, h3;
    LONG e[3];

    e[0] = 0xcc; e[1] = 0; e[2] = 0;
    h1 = AddVectoredExceptionHandler(CALL_FIRST, VectoredHandlerSkip1);
    IllegalInst();
    assert_match("Test2a", e, Actual);
    RemoveVectoredExceptionHandler(h1);
    check_all_zeros();

	printf("\n");

    e[0] = 1; e[1] = 2; e[2] = 0xcc;
    h2 = AddVectoredExceptionHandler(CALL_FIRST, VectoredHandler2);
    h3 = AddVectoredExceptionHandler(CALL_LAST, VectoredHandlerSkip3);
    h1 = AddVectoredExceptionHandler(CALL_FIRST, VectoredHandler1);
    IllegalInst();
    assert_match("Test2b", e, Actual);
    RemoveVectoredExceptionHandler(h1);
    RemoveVectoredExceptionHandler(h2);
    RemoveVectoredExceptionHandler(h3);
    check_all_zeros();

	printf("\n");

    e[0] = 1; e[1] = 0xcc; e[2] = 0;
    h1 = AddVectoredExceptionHandler(CALL_LAST, VectoredHandler1);
    h2 = AddVectoredExceptionHandler(CALL_LAST, VectoredHandlerSkip2);
    h3 = AddVectoredExceptionHandler(CALL_LAST, VectoredHandler3);
    IllegalInst();
    assert_match("Test2c", e, Actual);
    RemoveVectoredExceptionHandler(h1);
    RemoveVectoredExceptionHandler(h2);
    RemoveVectoredExceptionHandler(h3);
    check_all_zeros();

	printf("\n");

    e[0] = 2; e[1] = 0xcc; e[2] = 1;
    h1 = AddVectoredExceptionHandler(CALL_LAST, VectoredHandler1);
    h3 = AddVectoredExceptionHandler(CALL_FIRST, VectoredHandler3);
    h2 = AddVectoredExceptionHandler(CALL_LAST, VectoredHandlerSkip2);
    __try
    {
        IllegalInst();
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        // Should not make it to here.
        e[0] = 0; e[1] = 0; e[2] = 0;
        assert_match("Test2d-1", e, Actual);
    }
    assert_match("Test2d-2", e, Actual);
    RemoveVectoredExceptionHandler(h1);
    RemoveVectoredExceptionHandler(h2);
    RemoveVectoredExceptionHandler(h3);
    check_all_zeros();

	printf("\n");
}

void main()
{
    Test1();
    Test2();
}
