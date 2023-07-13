#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

// Prototype for function exported from VectoredExcBP DLL.
extern"C" void VectoredExcBP_ExportedAPI(void);

int main()
{
//	test_monitoring_myfunc();

	// Load a couple of DLLs, which in turn also call LoadLibrary on
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
}
