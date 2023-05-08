#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "iversion.h"
#include "MyLib.h"


bool MyLib_getversion(struct MyLib_version_st *pver)
{
	if(!pver)
		return false;

	pver->major = MyLib_vmajor;
	pver->minor = MyLib_vminor;
	pver->patch = MyLib_vpatch;
	pver->extra = MyLib_vextra;

	return true;
}


void MyLib_printversion()
{
	MyLib_version_st vst = {};
	MyLib_getversion(&vst);
	printf("MyLib version: %d.%d.%d.%d\n", vst.major, vst.minor, vst.patch, vst.extra);
}
