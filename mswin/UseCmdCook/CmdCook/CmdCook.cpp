//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "iversion.h"
#include "CmdCook.h"

DLLEXPORT_CmdCook
bool DLL_CALLCONV CmdCook_getversion(struct CmdCook_version_st *pver)
{
	if(!pver)
		return false;

	pver->major = CmdCook_vmajor;
	pver->minor = CmdCook_vminor;
	pver->patch = CmdCook_vpatch;
	pver->extra = CmdCook_vextra;

	return true;
}

DLLEXPORT_CmdCook
void DLL_CALLCONV CmdCook_printversion()
{
	CmdCook_version_st vst = {};
	CmdCook_getversion(&vst);
	printf("CmdCook version: %d.%d.%d.%d\n", vst.major, vst.minor, vst.patch, vst.extra);
}
