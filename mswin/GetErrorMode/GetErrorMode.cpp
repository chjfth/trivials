#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h>

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	DWORD errmode = GetErrorMode();
	_tprintf(_T("GetErrorMode()=0x%04X\n"), errmode);
	
	if(argc>1)
	{
		int sleepsec = _ttoi(argv[1]);
		_tprintf(_T("Sleep %d seconds then quit.\n"), sleepsec);
		Sleep(sleepsec*2000);
	}
	
	return 0;
}

