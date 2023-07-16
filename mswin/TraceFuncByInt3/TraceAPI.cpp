//===========================================================================
// [2023-07-13] Chj: This code is adapted from VectoredExcBP.cpp, 
// provided by Matt Pietrek, on MSDN Magazine, September 2001.
//
// The API Trace_UserFunc() accepts a function pointer(address of the first
// instruction of the function), then monitors that function by wedging an
// `int 3` (x86/x64) instruction at that address. That `int 3` would cause
// int3_BreakpointHandler() callback so the user-func is monitored.
// In short, this API works as if it is a debugger that have set a breakpoint
// on the first instruction of the monitored user-func.
//
// Demo program limitation: 
// (1) Only *one* user-func can be monitored.
// (2) To See *full* effects run the Debug-build exe from a CMD command-line.
//     Do NOT run under a Visual Studio debugger, which causes different 
//     behavior.
//
//===========================================================================

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>

#include "TraceAPI.h"

#if !defined(_M_IX86) && !defined(_M_X64)
#error "This code only runs on an x86 or X64 machine."
#endif

LONG NTAPI int3_BreakpointHandler(PEXCEPTION_POINTERS pExceptionInfo );
void BreakpointCallback( PVOID pCodeAddr, PVOID pStackAddr );
BYTE SetBreakpoint( PVOID pAddr );
void RemoveBreakpoint( PVOID pAddr, BYTE bOriginalOpcode );

// Global variables
PVOID g_pfnMonitored = nullptr;
BYTE g_originalCodeByte = 0;

HANDLE g_hVectHandler = nullptr; // handle of our installed exception-handler


void Trace_UserFunc(void *userfunc)
{
	g_pfnMonitored = userfunc;

	// Add a vectored exception handler for our breakpoint
	g_hVectHandler = AddVectoredExceptionHandler( 1, int3_BreakpointHandler );

	// Set the breakpoint on function entry.
	g_originalCodeByte = SetBreakpoint( userfunc );
}

void Untrace_UserFunc(void *userfunc)
{
	assert(userfunc==g_pfnMonitored);

	RemoveBreakpoint( g_pfnMonitored, g_originalCodeByte );
	
	RemoveVectoredExceptionHandler(g_hVectHandler);
	g_hVectHandler = nullptr;
}

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// Handler for our breakpoint handler.  When the 
// breakpoint is hit, invoke the callback function (BreakpointCallback). 
// Then step the original instruction and let execution continue.  This 
// actually requires that two exception be handled, as described in the 
// function's code.
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

LONG NTAPI int3_BreakpointHandler(PEXCEPTION_POINTERS pExceptionInfo)
{
	LONG exceptionCode = pExceptionInfo->ExceptionRecord->ExceptionCode;

	printf( "[Matt] In int3_BreakpointHandler: ExcpCode=0x%X EIP=%p\n",
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
			(PVOID)pExceptionInfo->ContextRecord->Rsp 
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
		// If not, pass on to other handlers.
		// Chj: roughly check for 4 bytes of machine instruction addr diff.
		//
		PVOID excpaddr = pExceptionInfo->ExceptionRecord->ExceptionAddress;
		int addr_diff = int( (INT_PTR)excpaddr - (INT_PTR)g_pfnMonitored );
#if 0
		// [2023-07-14] Actually, we should not check addr_diff, bcz it can be any value. 
		// If we int3-hook system API LoadLibraryExW, we may see addr_diff=-12 .
		if ( !(addr_diff>0 && addr_diff<=4) )
		{
			return EXCEPTION_CONTINUE_SEARCH;
		}
#endif
		printf("[Matt] Do STATUS_SINGLE_STEP handling. (addr_diff=%d)\n", addr_diff);

		// We've stepped the original instruction, so put the breakpoint back
		SetBreakpoint( g_pfnMonitored );

		//printf("[Matt] >>>> EFlags=0x%08X.\n", pExceptionInfo->ContextRecord->EFlags); 
		// -- Chj: I see 0x00000202, so EFlags's single-step bit already turned-off by system.

		// Turn off trace flag that we set above
		// pExceptionInfo->ContextRecord->EFlags &= ~0x00000100; // Chj: this is not necessary 

		return EXCEPTION_CONTINUE_EXECUTION;    // Continue on!
	}
	else    // Not a breakpoint or single step.  Definitely not ours!
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
}

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
// Invoked when user-func is called.  Passed the address of the
// breakpoint, and the stack pointer.  The stack pointer can be used
// to retrieve parameter values from the stack.  
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void BreakpointCallback( PVOID pCodeAddr, PVOID pStackAddr )
{
	SIZE_T nBytes = 0;
	LPWSTR pwszDllName = 0;

	// pStackAddr+0 == return address  (x86)
	// pStackAddr+4 == first parameter (x86)
	ReadProcessMemory( GetCurrentProcess(),
		(PVOID)((DWORD_PTR)pStackAddr+4),
		&pwszDllName, sizeof(pwszDllName),
		&nBytes );

//	printf("[Matt] LoadLibrary called on: %ls\n", pwszDllName);
	printf("[Matt] ## Monitored function is being called.\n");
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
