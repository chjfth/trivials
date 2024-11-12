#ifndef __MyDLL1_h_
#define __MyDLL1_h_

#ifdef __cplusplus
extern"C" {
#endif


// Note for compiling for Windows using Visual C++:
// The project file should pass to cl.exe 
//
//		/D DLLEXPORT_MyDLL1=__declspec(dllexport)
//
// so to have the interface functions "exported", even in the case
// that MyDLL1.def file is not passed to linker.

#ifndef DLLEXPORT_MyDLL1
#define DLLEXPORT_MyDLL1
#endif

#ifndef DLL_CALLCONV // DLL function calling convention
#ifdef _WIN32 // target Win32/Win64 OS
#define DLL_CALLCONV __stdcall
#else
#define DLL_CALLCONV
#endif
#endif
	
struct MyDLL1_version_st 
{
	int major, minor, patch, extra;
};

DLLEXPORT_MyDLL1
bool DLL_CALLCONV MyDLL1_getversion(struct MyDLL1_version_st *pver);

DLLEXPORT_MyDLL1
void DLL_CALLCONV MyDLL1_printversion();



#ifdef __cplusplus
} // extern"C" {
#endif

#endif
