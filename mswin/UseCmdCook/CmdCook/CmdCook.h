#ifndef __CmdCook_h_
#define __CmdCook_h_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#ifdef __cplusplus
extern"C" {
#endif


// Note for compiling for Windows using Visual C++:
// The project file should pass to cl.exe 
//
//		/D DLLEXPORT_CmdCook=__declspec(dllexport)
//
// so to have the interface functions "exported", even in the case
// that CmdCook.def file is not passed to linker.

#ifndef DLLEXPORT_CmdCook
#define DLLEXPORT_CmdCook
#endif

#ifndef DLL_CALLCONV // DLL function calling convention
#ifdef _WIN32 // target Win32/Win64 OS
#define DLL_CALLCONV __stdcall
#else
#define DLL_CALLCONV
#endif
#endif
	
struct CmdCook_version_st 
{
	int major, minor, patch, extra;
};

DLLEXPORT_CmdCook
bool DLL_CALLCONV CmdCook_getversion(struct CmdCook_version_st *pver);

DLLEXPORT_CmdCook
void DLL_CALLCONV CmdCook_printversion();

DLLEXPORT_CmdCook
void DLL_CALLCONV Cook1(HWND hwnd, HINSTANCE hinst, LPSTR lpszCmdLine, int nCmdShow);


#ifdef __cplusplus
} // extern"C" {
#endif

#endif
