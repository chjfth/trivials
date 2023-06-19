//==================================================
// MYSEH2.CPP - Matt Pietrek 1997
// Microsoft Systems Journal, January 1997
// To compile: CL MYSEH2.CPP
//==================================================
/* [2023-06-18] Chj memo:
  
  With VC7.1+, In order to see _except_handler() being executed , 
  we need to assign linker option /SAFESEH:NO , otherwise, Windows would
  just neglect our _except_handler() handler. 
  
  With VC7.1+, we will always see a warning message from cl.exe :

  warning C4733: Inline asm assigning to 'FS:0' : handler not registered as safe handler

  We are doing this by purpose, so just ignore that warning.
*/
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

EXCEPTION_DISPOSITION __cdecl
_except_handler(
	struct _EXCEPTION_RECORD *ExceptionRecord,
	void * EstablisherFrame, // = pointer to a EXCEPTION_REGISTRATION
	struct _CONTEXT *ContextRecord,
	void * DispatcherContext 
	)
{
	printf( "Home Grown handler: Exception Code: %08X Exception Flags %X",
		ExceptionRecord->ExceptionCode, ExceptionRecord->ExceptionFlags );

	if ( ExceptionRecord->ExceptionFlags & 1 )
		printf( " EH_NONCONTINUABLE" );
	if ( ExceptionRecord->ExceptionFlags & 2 )
		printf( " EH_UNWINDING" );
	if ( ExceptionRecord->ExceptionFlags & 4 )
		printf( " EH_EXIT_UNWIND" );
	if ( ExceptionRecord->ExceptionFlags & 8 )
		printf( " EH_STACK_INVALID" );
	if ( ExceptionRecord->ExceptionFlags & 0x10 )
		printf( " EH_NESTED_CALL" );

	printf( "\n" );

	// Punt... We don't want to handle this... Let somebody else handle it
	return ExceptionContinueSearch;
}

void HomeGrownFrame( void )
{
	DWORD handler = (DWORD)_except_handler;

	__asm
	{                           // Build EXCEPTION_REGISTRATION record:
		push    handler         // Address of handler function
		push    FS:[0]          // Address of previous handler
		mov     FS:[0],ESP      // Install new EXECEPTION_REGISTRATION
	}

	*(PDWORD)0 = 0;             // Write to address 0 to cause a fault

	printf( "I should never get here!\n" );

	__asm
	{                           // Remove our EXECEPTION_REGISTRATION record
		mov     eax,[ESP]       // Get pointer to previous record
		mov     FS:[0], EAX     // Install previous record
		add     esp, 8          // Clean our EXECEPTION_REGISTRATION off stack
	}
}

int main()
{
	__try
	{
		HomeGrownFrame(); 
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		printf( "Caught the exception in main()\n" );
	}

	return 0;
}
