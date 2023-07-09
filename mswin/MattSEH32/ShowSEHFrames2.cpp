// Test program based on ShowSEHFrame.cpp
// Difference at Function2x(). This function have parallel __try and nested __try.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#pragma hdrstop

//----------------------------------------------------------------------------
// !!! WARNING !!!  This program only works with Visual C++, as the data
// structures being shown are specific to Visual C++.
//----------------------------------------------------------------------------

#ifndef _MSC_VER
#error Visual C++ Required (Visual C++ specific information is displayed)
#endif

void WalkSEHFrames( void );

bool g_force_print = false;

//----------------------------------------------------------------------------
// Structure Definitions
//----------------------------------------------------------------------------

// The basic, OS defined exception frame

struct EXCEPTION_REGISTRATION
{
	EXCEPTION_REGISTRATION* prev;
	FARPROC                 handler;
};

// Data structure(s) pointed to by Visual C++ extended exception frame

struct scopetable_entry
{
	DWORD       previousTryLevel;
	FARPROC     lpfnFilter;
	FARPROC     lpfnHandler;
};

// The extended exception frame used by Visual C++

struct VC_EXCEPTION_REGISTRATION : EXCEPTION_REGISTRATION
{
	scopetable_entry *  scopetable;
	int                 trylevel;
	int                 _ebp;
};

//----------------------------------------------------------------------------
// Prototypes
//----------------------------------------------------------------------------

// _except_handler3 is a Visual C++ RTL function.  We want to refer to
// it in order to print it's address.  However, we need to prototype it since
// it doesn't appear in any header file.

extern "C" int _except_handler3(PEXCEPTION_RECORD, EXCEPTION_REGISTRATION *,
	PCONTEXT, PEXCEPTION_RECORD);


//----------------------------------------------------------------------------
// Code
//----------------------------------------------------------------------------

//
// Display the information in one exception frame, along with its scopetable
//

void ShowSEHFrame( VC_EXCEPTION_REGISTRATION * pVCExcRec )
{
	printf( "Frame: %08X  Handler: %08X  Prev: %08X  Scopetable: %08X\n",
		(UINT32)pVCExcRec, (UINT32)pVCExcRec->handler, 
		(UINT32)pVCExcRec->prev, (UINT32)pVCExcRec->scopetable );

	static bool s_run1 = false;
	if(pVCExcRec->handler != (void*)_except_handler3)
	{
		if(g_force_print)
		{
			printf("    Not _except_handler3, but force-print it (will cause real exception on Win7+).\n");
		}
		else
		{
			// Chj: We do not recognize this handler( _except_handler4 in VC2010 etc ).
			printf( "    Not _except_handler3, cannot interpret.\n" );

			if(!s_run1)
			{
				printf( "    (Hint: to meet _except_handler3, please use /GS- compiler option.)\n" );
			}

			printf("\n");
			s_run1 = true;
			return;
		}
	}
	s_run1 = true;

	scopetable_entry * pScopeTableEntry = pVCExcRec->scopetable;

	for ( int i = 0; i <= pVCExcRec->trylevel; i++ )
	{
		printf( "    scopetable[%u] PrevTryLevel: %08X  filter: %08X  __except: %08X\n", 
			i,
			pScopeTableEntry->previousTryLevel,
			(UINT32)pScopeTableEntry->lpfnFilter,
			(UINT32)pScopeTableEntry->lpfnHandler );

		pScopeTableEntry++;
	}

	printf( "\n" );
}   

//
// Walk the linked list of frames, displaying each in turn
//

void WalkSEHFrames( void )
{
	VC_EXCEPTION_REGISTRATION * pVCExcRec;

	// Print out the location of the __except_handler3 function
	printf( "_except_handler3 is at address: %08X\n", (UINT32)_except_handler3 );
	printf( "\n" );

	// Get a pointer to the head of the chain at FS:[0]
	__asm   mov eax, FS:[0]
	__asm   mov [pVCExcRec], EAX

	// Walk the linked list of frames.  0xFFFFFFFF indicates the end of list
	while (  0xFFFFFFFF != (unsigned)pVCExcRec )
	{
		ShowSEHFrame( pVCExcRec );
		pVCExcRec = (VC_EXCEPTION_REGISTRATION *)(pVCExcRec->prev);
	}       
}

void Function2x(void)
{
	// Set up 2 nested __try levels 
	int ii, jj;
	__try
	{
		ii = 0x1234;

		__try
		{
			jj = 0x5678;
		}
		__except (EXCEPTION_CONTINUE_SEARCH)
		{
			jj = 0x8765;
		}
	}
	__except (EXCEPTION_CONTINUE_SEARCH)
	{
		ii = 0x4321;
	}


	// Set up 3 nested __try levels 
	__try
	{
		__try
		{
			__try
			{
				WalkSEHFrames();
			}
			__except (EXCEPTION_CONTINUE_SEARCH)
			{
			}
		}
		__except (EXCEPTION_CONTINUE_SEARCH)
		{
		}
	}
	__except (EXCEPTION_CONTINUE_SEARCH)
	{
	}

}

int main(int argc, char *argv[])
{
	if(argc>1 && _stricmp(argv[1], "force")==0)
	{
		g_force_print = true;
	}

	int i = 0;

	// Use two (non-nested) __try blocks.  This causes two scopetable entries
	// to be generated for the function.

	__try
	{
		i = 0x1234;     // Do nothing in particular
	}
	__except( EXCEPTION_CONTINUE_SEARCH )
	{
		i = 0x4321;     // Do nothing (in reverse)
	}

	__try
	{
		Function2x();    // Call a function that sets up more exception frames
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		// Should never get here, since we aren't expecting an exception
		printf( "Oops! Caught Exception in main()!\n" );
	}

	return 0;
}
