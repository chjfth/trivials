#ifndef __chjdll_h_
#define __chjdll_h_

#ifdef __cplusplus
extern"C" {
#endif


#ifndef DLLEXPORT_chjdll
#define DLLEXPORT_chjdll
#endif

#ifndef DLL_CALLCONV // DLL function calling convention
#define DLL_CALLCONV __stdcall
#endif
	
struct chjdll_version_st 
{
	int major, minor, patch, extra;
};

DLLEXPORT_chjdll
double DLL_CALLCONV SumHalf(int *ptr, int size);

DLLEXPORT_chjdll
bool DLL_CALLCONV chjdll_getversion(struct chjdll_version_st *pver);

DLLEXPORT_chjdll
void DLL_CALLCONV chjdll_printversion();



#ifdef __cplusplus
} // extern"C" {
#endif

#endif
