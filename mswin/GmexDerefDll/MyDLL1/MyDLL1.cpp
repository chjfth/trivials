//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "iversion.h"
#include "MyDLL1.h"

DLLEXPORT_MyDLL1
bool DLL_CALLCONV MyDLL1_getversion(struct MyDLL1_version_st *pver)
{
	if(!pver)
		return false;

	pver->major = MyDLL1_vmajor;
	pver->minor = MyDLL1_vminor;
	pver->patch = MyDLL1_vpatch;
	pver->extra = MyDLL1_vextra;

	return true;
}

DLLEXPORT_MyDLL1
void DLL_CALLCONV MyDLL1_printversion()
{
	MyDLL1_version_st vst = {};
	MyDLL1_getversion(&vst);
	printf("MyDLL1 version: %d.%d.%d.%d\n", vst.major, vst.minor, vst.patch, vst.extra);
}
