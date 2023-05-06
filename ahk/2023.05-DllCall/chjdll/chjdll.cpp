#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "iversion.h"
#include "chjdll.h"

DLLEXPORT_chjdll
double DLL_CALLCONV SumHalf(int *ptr, int size)
{
	WCHAR buf[100];
	wsprintf(buf, L"SumHalf() sees ptr=0x%p\r\n", ptr);
	OutputDebugStringW(buf);

	double dret = (double)(*ptr + size)/2;

	_snwprintf_s(buf, _TRUNCATE, 
		L"SumHalf()     *ptr=%d , size=%d, return: %g\r\n", 
		*ptr, size, dret);
	OutputDebugStringW(buf);

	return dret;
}

DLLEXPORT_chjdll
bool DLL_CALLCONV chjdll_getversion(struct chjdll_version_st *pver)
{
	if(!pver)
		return false;

	pver->major = chjdll_vmajor;
	pver->minor = chjdll_vminor;
	pver->patch = chjdll_vpatch;
	pver->extra = chjdll_vextra;

	return true;
}

DLLEXPORT_chjdll
void DLL_CALLCONV chjdll_printversion()
{
	chjdll_version_st vst = {};
	chjdll_getversion(&vst);
	printf("chjdll version: %d.%d.%d.%d\n", vst.major, vst.minor, vst.patch, vst.extra);
}
