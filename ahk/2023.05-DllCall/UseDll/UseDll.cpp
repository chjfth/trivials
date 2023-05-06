#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

#include <chjdll.h>

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	chjdll_version_st vst = {};
	chjdll_getversion(&vst);

	_tprintf(_T("EXE: Using chjdll version: %d.%d.%d.%d\n"), vst.major, vst.minor, vst.patch, vst.extra);

	int input1 = 10;
	double sumhalf = SumHalf(&input1, 5);
	
	_tprintf(_T("sumhalf is %g\n"), sumhalf);

	return 0;
}

