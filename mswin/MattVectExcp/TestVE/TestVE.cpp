#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

// Prototype for function exported from VectoredExcBP DLL.
extern"C" void VectoredExcBP_ExportedAPI(void);

int main(int argc, char *argv[])
{
	if(argc>1 && argv[1][0]=='1')
		DebugBreak();

	// Load a couple of DLLs, which in turn (may) also call LoadLibrary on
	// other DLLs.

	const TCHAR *dllname = _T("MFC42");
	_tprintf(_T("LoadLibrary(%s) ...\n"), dllname);
	LoadLibrary(dllname);

	dllname =  _T("WININET");
	_tprintf(_T("LoadLibraryEx(%s) ...\n"), dllname);
	LoadLibraryEx(dllname, 0, 0);

	dllname =  _T("SetupAPI");
	_tprintf(_T("LoadLibraryEx(%s) ...\n"), dllname);
	LoadLibraryEx(dllname, 0, 0);

	// Call exported function in VectoredExcBP DLL.  This is simply
	// to force that DLL to be loaded.
	VectoredExcBP_ExportedAPI();

	if(argc>2 && argv[2][0]=='1')
		DebugBreak();

	return 0;
}
