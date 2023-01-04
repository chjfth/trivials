#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h>

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	DWORD tc32 = GetTickCount();
	ULONGLONG tc64 = GetTickCount64();

	_tprintf(_T("32-bit GetTickCount()   = 0x%-8X , %u\n"), tc32, tc32);
	_tprintf(_T("64-bit GetTickCount64() = 0x%-8I64X , %I64u\n"), tc64, tc64);

	return 0;
}

