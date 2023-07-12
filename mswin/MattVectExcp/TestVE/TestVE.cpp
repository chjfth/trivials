#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

// Prototype for function exported from VectoredExcBP DLL.
extern "C" void VectoredExcBP_ExportedAPI(void);

int main()
{
	// Load a couple of DLLs, which in turn also call LoadLibrary on
	// other DLLs.
	LoadLibrary( _T("MFC42") );

	LoadLibrary( _T("WININET") );

	// Call exported function in VectoredExcBP DLL.  This is simply
	// to force that DLL to be loaded.
	VectoredExcBP_ExportedAPI();

	return 0;
}
