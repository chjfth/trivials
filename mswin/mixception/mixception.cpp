#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

int do_divide(int dividend, int divisor)
{
	int result = dividend / divisor;
	return result;
}

int do_work()
{
	const TCHAR* pz = GetCommandLine();
	int slen = (int)_tcslen(pz);

	int divisor = 0;
	if (pz[slen - 1] == '1') // if command-line end with param "1"
		divisor = 1;

	int result = 6 / divisor;
	return result;
}

int _tmain(int argc, TCHAR* argv[])
{
	return do_work();
}

int __stdcall _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int)
{
	return do_work();
}
