//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "iversion.h"
#include "MyDLL2.h"

DLLEXPORT_MyDLL2
bool DLL_CALLCONV MyDLL2_getversion(struct MyDLL2_version_st *pver)
{
	if(!pver)
		return false;

	pver->major = MyDLL2_vmajor;
	pver->minor = MyDLL2_vminor;
	pver->patch = MyDLL2_vpatch;
	pver->extra = MyDLL2_vextra;

	return true;
}

DLLEXPORT_MyDLL2
void DLL_CALLCONV MyDLL2_printversion()
{
	MyDLL2_version_st vst = {};
	MyDLL2_getversion(&vst);
	printf("MyDLL2 version: %d.%d.%d.%d\n", vst.major, vst.minor, vst.patch, vst.extra);
}
