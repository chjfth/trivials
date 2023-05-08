#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "iversion.h"
#include "MyDll.h"

DLLEXPORT_MyDll
bool DLL_CALLCONV MyDll_getversion(struct MyDll_version_st *pver)
{
	if(!pver)
		return false;

	pver->major = MyDll_vmajor;
	pver->minor = MyDll_vminor;
	pver->patch = MyDll_vpatch;
	pver->extra = MyDll_vextra;

	return true;
}

DLLEXPORT_MyDll
void DLL_CALLCONV MyDll_printversion()
{
	MyDll_version_st vst = {};
	MyDll_getversion(&vst);
	printf("MyDll version: %d.%d.%d.%d\n", vst.major, vst.minor, vst.patch, vst.extra);
}
