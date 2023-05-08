#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

#include <MyDll.h>

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	MyDll_version_st vst = {};
	MyDll_getversion(&vst);

	_tprintf(_T("EXE: Using MyDll version: %d.%d.%d.%d\n"), vst.major, vst.minor, vst.patch, vst.extra);

	return 0;
}

