//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "iversion.h"
#include "CmdCook.h"

DLLEXPORT_CmdCook
bool DLL_CALLCONV CmdCook_getversion(struct CmdCook_version_st *pver)
{
	if(!pver)
		return false;

	pver->major = CmdCook_vmajor;
	pver->minor = CmdCook_vminor;
	pver->patch = CmdCook_vpatch;
	pver->extra = CmdCook_vextra;

	return true;
}

DLLEXPORT_CmdCook
void DLL_CALLCONV CmdCook_printversion()
{
	CmdCook_version_st vst = {};
	CmdCook_getversion(&vst);
	printf("CmdCook version: %d.%d.%d.%d\n", vst.major, vst.minor, vst.patch, vst.extra);
}

void dbgprint(const char *fmt, ...)
{
	char buf[1000] = {0};

	va_list args;
	va_start(args, fmt);
	
	_vsnprintf_s(buf, _TRUNCATE, fmt, args);
	
	va_end(args);

	OutputDebugStringA(buf);
	printf("%s\n", buf);
}


DLLEXPORT_CmdCook
void DLL_CALLCONV Cook1(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow)
{
	// Note: 
	// lpszCmdLine is always a narrow string.
	//	hinst will be the the module address of rundll32.exe .

	dbgprint("[Cook1] hwnd=0x%08X, hinst=0x%X, lpszCmdLine=%s", 
		hwnd, hinst, lpszCmdLine);
}
