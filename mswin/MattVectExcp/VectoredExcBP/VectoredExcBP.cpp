//===========================================================================
// VectoredExcBP - Matt Pietrek 2001
// MSDN Magazine, September 2001
//
// !^!^!^! WARNING WARNING WARNING !^!^!^!
//  This code requires Windows XP or later.
//  To compile this code correctly, you must:
//      A) Have a WINBASE.H that defines AddVectoredExceptionHandler
//      B) Make sure that the compiler finds *that* WINBASE.H before any
//          other older versions of WINBASE.H.  See the compiler 
//          documentation if you're not sure how to do this.
//      C) Define _WIN32_WINNT=0x0500   (or higher)
//
// This code compiles and works correctly with Beta 2 of Windows XP and
// Visual C++ 6.0.  At the time of this writing, Windows XP is in beta,
// so things could change, including API behavior, etc...  No guarantees
// are made that this code will work in the future.
//
// [2023-07-14] Jimm Chen does some FIX for WinXP SP3 and Win7+ :
// For STATUS_SINGLE_STEP, addr_diff can be any value, so do NOT check it. 
// This program now runs both as x86 and x64 exe.
//
//===========================================================================

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#if !defined(_M_IX86) && !defined(_M_X64)
#error "This code only runs on an x86 or X64 machine."
#endif

LONG NTAPI LoadLibraryBreakpointHandler(PEXCEPTION_POINTERS pExceptionInfo );
void BreakpointCallback( PVOID pCodeAddr, PVOID pStackAddr );
void SetupLoadLibraryExWCallback(void);
BYTE SetBreakpoint( PVOID pAddr );
void RemoveBreakpoint( PVOID pAddr, BYTE bOriginalOpcode );

// Global variables
PVOID g_pfnMonitored = 0;
BYTE g_originalCodeByte = 0;

HANDLE g_hVectHandler = NULL; // handle of our installed exception-handler

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	// We don't need thread start/stop notifications, so disable them
	DisableThreadLibraryCalls( (HINSTANCE)hModule );

	// At startup, set LoadLibrary breakpoint, at shutdown, remove it
	if ( DLL_PROCESS_ATTACH == ul_reason_for_call )
	{
		SetupLoadLibraryExWCallback();
	}
	else if ( DLL_PROCESS_DETACH == ul_reason_for_call )
	{
		RemoveBreakpoint( g_pfnMonitored, g_originalCodeByte );

		RemoveVectoredExceptionHandler(g_hVectHandler);
		g_hVectHandler = NULL;
	}
	return TRUE;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void SetupLoadLibraryExWCallback(void)
{
	// Obtain the address of LoadLibraryExW.  
	// All LoadLibraryA/W/ExA calls
	// go through LoadLibraryExW
	g_pfnMonitored = (PVOID)GetProcAddress(
		GetModuleHandle(_T("KERNEL32")), "LoadLibraryExW" ); // chj: not using LoadLibraryExW on Win7

	// Add a vectored exception handler for our breakpoint
	g_hVectHandler = AddVectoredExceptionHandler( 1, LoadLibraryBreakpointHandler );

	// Set the breakpoint on LoadLibraryExW.
	g_originalCodeByte = SetBreakpoint( g_pfnMonitored );
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// Handler for our LoadLibraryExW breakpoint handler.  When the 
// breakpoint is hit, invoke the callback function (BreakpointCallback). 
// Then step the original instruction and let execution continue.  This 
// actually requires that two exception be handled, as described in the 
// function's code
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LONG NTAPI LoadLibraryBreakpointHandler(PEXCEPTION_POINTERS pExceptionInfo )
{
	LONG exceptionCode = pExceptionInfo->ExceptionRecord->ExceptionCode;

	printf( "[Matt] ==== In LoadLibraryBreakpointHandler: ExcpCode=0x%X EIP=%p\n",
		exceptionCode, pExceptionInfo->ExceptionRecord->ExceptionAddress );

	if ( exceptionCode == STATUS_BREAKPOINT)
	{
		// Make sure it's our breakpoint.  If not, pass on to other handlers
		if ( pExceptionInfo->ExceptionRecord->ExceptionAddress != g_pfnMonitored )
		{
			return EXCEPTION_CONTINUE_SEARCH;
		}

		printf("[Matt] Do STATUS_BREAKPOINT handling.\n");

		// We need to step the original instruction, so temporarily 
		// remove the breakpoint
		RemoveBreakpoint( g_pfnMonitored, g_originalCodeByte );

		// Call our code to do whatever processing desired at this point
		BreakpointCallback( pExceptionInfo->ExceptionRecord->ExceptionAddress,
#ifdef _M_IX86
			(PVOID)pExceptionInfo->ContextRecord->Esp 
#else
			(PVOID)pExceptionInfo->ContextRecord->Rcx // the first parameter value for x64 ABI
#endif
			);

		// Set trace flag in the EFlags register so that only one 
		// instruction  will execute before we get a STATUS_SINGLE_STEP
		// (see below)
		pExceptionInfo->ContextRecord->EFlags |= 0x00000100;

		return EXCEPTION_CONTINUE_EXECUTION;    // Restart the instruction
	}
	else if ( exceptionCode == STATUS_SINGLE_STEP )
	{
		// Make sure the exception address is the single step we caused above.
		// If not, pass on to other handlers
		PVOID excpaddr = pExceptionInfo->ExceptionRecord->ExceptionAddress;
		int addr_diff = int( (INT_PTR)excpaddr - (INT_PTR)g_pfnMonitored );

		// Jimm: We should not check this, addr_diff can be any value.
		// * 2 on WinXP SP3
		// * -12 on Win7 x64, 64-bit process.
// 		if ( !(addr_diff>0 && addr_diff<=4) )
// 		{
// 			return EXCEPTION_CONTINUE_SEARCH;
// 		}

		printf("[Matt] Do STATUS_SINGLE_STEP handling. (addr_diff=%d)\n", addr_diff);

		// We've stepped the original instruction, so put the breakpoint back
		SetBreakpoint( g_pfnMonitored );

		// Turn off trace flag that we set above 
		// ( Jimm: This is not required, it is automatically off. )
		// pExceptionInfo->ContextRecord->EFlags &= ~0x00000100;

		return EXCEPTION_CONTINUE_EXECUTION;    // Continue on!
	}
	else    // Not a breakpoint or single step.  Definitely not ours!
	{
//		printf("[Matt] See excpcode 0x%0X , bypass it.\n", exceptionCode);

		return EXCEPTION_CONTINUE_SEARCH;
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// Invoked when LoadLibraryExW is called.  Passed the address of the
// breakpoint, and the stack pointer.  The stack pointer can be used
// to retrieve parameter values from the stack.  In this case, we want
// to retrieve the unicode string that names to DLL to be loaded.
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void BreakpointCallback( PVOID pCodeAddr, PVOID pStackAddr )
{
	SIZE_T nBytes = 0;
	LPWSTR pwszDllName = 0;

#ifdef _M_IX86
	// pStackAddr+0 == return address
	// pStackAddr+4 == first parameter
	ReadProcessMemory(  GetCurrentProcess(),
		(PVOID)((DWORD_PTR)pStackAddr+4),
		&pwszDllName, sizeof(pwszDllName),
		&nBytes );

	printf("[Matt] # LoadLibrary called on: %ls\n", pwszDllName);
#else
//	DebugBreak();
	PVOID first_param = pStackAddr;
	printf("[Matt] # LoadLibrary called on: %ls\n", first_param);
#endif
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// Sets breakpoint at specific address, returns original opcode byte where
// breakpoint was set.
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
BYTE SetBreakpoint( PVOID pAddr )
{
	SIZE_T nBytes = 0;
	BYTE bOriginalOpcode = 0;

	// Read the byte at the specified address
	ReadProcessMemory( GetCurrentProcess(), pAddr,
		&bOriginalOpcode, sizeof(bOriginalOpcode),
		&nBytes);

	// Write breakpoint
	BYTE bpOpcode = 0xCC;
	WriteProcessMemory( GetCurrentProcess(), pAddr,
		&bpOpcode, sizeof(bpOpcode),
		&nBytes );

	return bOriginalOpcode; 
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// Writes the original opcode byte back to the specified address where
// a breakpoint was previously written.
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void RemoveBreakpoint( PVOID pAddr, BYTE bOriginalOpcode )
{
	SIZE_T nBytes = 0;

	WriteProcessMemory( GetCurrentProcess(),
		pAddr,
		&bOriginalOpcode, sizeof(bOriginalOpcode),
		&nBytes );
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
extern "C" void __declspec(dllexport) VectoredExcBP_ExportedAPI(void)
{
	// Do nothing function.  Just exported so that an EXE can link
	// against this DLL.
}
