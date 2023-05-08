#ifndef __MyDll_h_
#define __MyDll_h_

#ifdef __cplusplus
extern"C" {
#endif


#ifndef DLLEXPORT_MyDll
#define DLLEXPORT_MyDll
#endif

#ifndef DLL_CALLCONV // DLL function calling convention
#ifdef _WIN32 // target Win32/Win64 OS
#define DLL_CALLCONV __stdcall
#else
#define DLL_CALLCONV
#endif
#endif
	
struct MyDll_version_st 
{
	int major, minor, patch, extra;
};

DLLEXPORT_MyDll
bool DLL_CALLCONV MyDll_getversion(struct MyDll_version_st *pver);

DLLEXPORT_MyDll
void DLL_CALLCONV MyDll_printversion();



#ifdef __cplusplus
} // extern"C" {
#endif

#endif
