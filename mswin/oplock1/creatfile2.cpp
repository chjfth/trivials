#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <conio.h>
#include <windows.h>
#include "share.h"

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	if(argc==1)
	{
		printf("Need at least a filename as parameter.\n");
		printf("  creatfile2 <file> <DesiredAccess> <ShareMode>\n");
		printf("\n");
		printf("Examples:\n");
		printf("  creatfile2 input.txt 0x80000000 1\n");
		printf("  creatfile2 input.txt 0xC0000000 3\n");
		printf("\n");
		printf("<DesiredAccess>:\n");
		printf("  0x80000000     GENERIC_READ\n");
		printf("  0x40000000     GENERIC_WRITE\n");
		printf("  0x20000000     GENERIC_EXECUTE\n");
		printf("  0x10000000     GENERIC_ALL\n");
		printf("<ShareMode>\n");
		printf("  1              FILE_SHARE_READ\n");
		printf("  2              FILE_SHARE_WRITE\n");
		printf("  4              FILE_SHARE_DELETE\n");
		
		exit(1);
	}
	
	DWORD DesiredAccess = GENERIC_READ;
	DWORD ShareMode = FILE_SHARE_READ;

	const TCHAR *szfn = argv[1];

	if(argc>2)
		DesiredAccess = _tcstoul(argv[2], nullptr, 0);

	if(argc>3)
		ShareMode = _tcstoul(argv[3], nullptr, 0);

	PrnTs(_T("Calling CreateFile()...\n")
		_T("    szfn = %s\n")
		_T("    DesiredAccess = 0x%08X\n")
		_T("    ShareMode     = 0x%X")
		, 
		szfn,
		DesiredAccess,
		ShareMode);

	HANDLE hfile = CreateFile(szfn,
		DesiredAccess,
		ShareMode,
		NULL, // no security attribute
		OPEN_EXISTING, // dwCreationDisposition
		FILE_FLAG_OVERLAPPED,
		NULL);
	if(hfile==INVALID_HANDLE_VALUE) {
		PrnTs(_T("File open error. WinErr=%d."), GetLastError());
		exit(4);
	}

	PrnTs(_T("CreateFile() success, hfile=0x%p."), hfile);

	_tprintf(_T("\n  Press Enter to close file.\n"));

	char tbuf[10] = "";
	fgets(tbuf, sizeof(tbuf), stdin);
	
	PrnTs(_T("Calling Closehandle()."));

	CloseHandle(hfile);

	PrnTs(_T("Done    Closehandle()."));

	return 0;
}

