#ifndef __VectoredExcBP_h_
#define __VectoredExcBP_h_

#ifdef __cplusplus
extern"C" {
#endif


// Note for compiling for Windows using Visual C++:
// The project file should pass to cl.exe 
//
//		/D DLLEXPORT_VectoredExcBP=__declspec(dllexport)
//
// so to have the interface functions "exported", even in the case
// that VectoredExcBP.def file is not passed to linker.

#ifndef DLLEXPORT_VectoredExcBP
#define DLLEXPORT_VectoredExcBP
#endif

#ifndef DLL_CALLCONV // DLL function calling convention
#ifdef _WIN32 // target Win32/Win64 OS
#define DLL_CALLCONV __stdcall
#else
#define DLL_CALLCONV
#endif
#endif
	
struct VectoredExcBP_version_st 
{
	int major, minor, patch, extra;
};

DLLEXPORT_VectoredExcBP
bool DLL_CALLCONV VectoredExcBP_getversion(struct VectoredExcBP_version_st *pver);

DLLEXPORT_VectoredExcBP
void DLL_CALLCONV VectoredExcBP_printversion();



#ifdef __cplusplus
} // extern"C" {
#endif

#endif
