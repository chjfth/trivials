#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <assert.h>

#include <windows.h>

#include <snTprintf.h>

void test_snprintf()
{
	TCHAR srcstr[] = _T("012345");
	int srclen = ARRAYSIZE(srcstr)-1;

	TCHAR szbuf[4];
	int c99ret1 = snTprintf(szbuf, ARRAYSIZE(szbuf), _T("%s"), srcstr);
	assert(c99ret1==srclen);

	int c99ret2 = snTprintf(szbuf, _T("%s"), srcstr);
	assert(c99ret2==srclen);
}


int _tmain(int argc, TCHAR* argv[])
{
	test_snprintf();
	return 0;
}

