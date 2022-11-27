#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>



int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	if(argc==1)
	{

	}

	_tprintf(_T("Hello, PipeClient1!\n"));
	_tprintf(_T("sizeof(TCHAR)=%d\n"), sizeof(TCHAR));
	return 0;
}

