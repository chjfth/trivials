//==================================================
// MYSEH1.CPP - Matt Pietrek 1997
// Microsoft Systems Journal, January 1997
// To compile: CL MYSEH1.CPP
//==================================================
/* [2023-06-18] Chj memo:
  
  With VC7.1+, In order to see "Hello from an exception handler" printed, 
  we need to assign linker option /SAFESEH:NO , otherwise, Windows would
  just neglect our _except_handler() handler. 
  
  With VC7.1+, we will always see a warning message from cl.exe :

  warning C4733: Inline asm assigning to 'FS:0' : handler not registered as safe handler

  We are doing this by purpose, so just ignore that warning.
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

DWORD  scratch = 0;

EXCEPTION_DISPOSITION __cdecl
_except_handler(
	struct _EXCEPTION_RECORD *ExceptionRecord,
	void * EstablisherFrame,
	struct _CONTEXT *ContextRecord,
	void * DispatcherContext 
	)
{
	unsigned i = 0;

	// Indicate that we made it to our exception handler
	printf( "Hello from an exception handler\n" );

	// Change EAX in the context record so that it points to someplace
	// where we can successfully write
	ContextRecord->Eax = (DWORD)&scratch;

	// Tell the OS to restart the faulting instruction
	return ExceptionContinueExecution;
}

int main()
{
	DWORD handler = (DWORD)_except_handler;

	__asm
	{                           // Build EXCEPTION_REGISTRATION record:
		push    handler         // Address of handler function
		push    FS:[0]          // Address of previous handler
		mov     FS:[0],ESP      // Install new EXECEPTION_REGISTRATION
	}

	__asm
	{
		mov     eax,0           // Zero out EAX
		mov     [eax], 1        // Write to EAX to deliberately cause a fault
	}

	printf( "After writing!\n" );

	__asm
	{                           // Remove our EXECEPTION_REGISTRATION record
		mov     eax,[ESP]       // Get pointer to previous record
		mov     FS:[0], EAX     // Install previous record
		add     esp, 8          // Clean our EXECEPTION_REGISTRATION off stack
	}

	return 0;
}

