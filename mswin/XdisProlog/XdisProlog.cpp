#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	_tprintf(_T("Hello, XdisProlog!\n"));
	_tprintf(_T("sizeof(TCHAR)=%d\n"), (int)sizeof(TCHAR));
	return 0;
}

