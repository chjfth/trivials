#ifndef __MyDLL2_h_
#define __MyDLL2_h_

#ifdef __cplusplus
extern"C" {
#endif


// Note for compiling for Windows using Visual C++:
// The project file should pass to cl.exe 
//
//		/D DLLEXPORT_MyDLL2=__declspec(dllexport)
//
// so to have the interface functions "exported", even in the case
// that MyDLL2.def file is not passed to linker.

#ifndef DLLEXPORT_MyDLL2
#define DLLEXPORT_MyDLL2
#endif

#ifndef DLL_CALLCONV // DLL function calling convention
#ifdef _WIN32 // target Win32/Win64 OS
#define DLL_CALLCONV __stdcall
#else
#define DLL_CALLCONV
#endif
#endif
	
struct MyDLL2_version_st 
{
	int major, minor, patch, extra;
};

DLLEXPORT_MyDLL2
bool DLL_CALLCONV MyDLL2_getversion(struct MyDLL2_version_st *pver);

DLLEXPORT_MyDLL2
void DLL_CALLCONV MyDLL2_printversion();



#ifdef __cplusplus
} // extern"C" {
#endif

#endif
