#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <conio.h>
#include <windows.h>
#include "share.h"

const TCHAR *g_version = _T("1.1");

void wait_enter()
{
	while(_getch()!='\r');
}

void do_writefile(HANDLE hfile, int pos, int size, const TCHAR *wpattern)
{
	char wbpattern[100] = {};
	WideCharToMultiByte(CP_ACP, 0, wpattern,-1, wbpattern, sizeof(wbpattern), NULL, NULL);

	int plen = (int)strlen(wbpattern);
	char *wbuf = new char[size+plen];
	int j=0;
	for(; j<size; j+=plen)
	{
		memcpy(wbuf+j, wbpattern, plen);
	}

	OVERLAPPED ovlp = {};
	ovlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ovlp.Offset = pos;

	PrnTs(_T("WriteFile: pos=%d bytes=%d pattern=%s"), pos, size, wpattern);

	DWORD nBytesDone = 0;
	BOOL succ = WriteFile(hfile, wbuf, size, &nBytesDone, &ovlp);
	DWORD winerr = GetLastError();
	if(!succ && winerr==ERROR_IO_PENDING)
	{
		succ = GetOverlappedResult(hfile, &ovlp, &nBytesDone, TRUE);
	}

	if(succ)
		PrnTs(_T("WriteFile success, written %d bytes."), nBytesDone);
	else
		PrnTs(_T("WriteFile fail, WinErr=%d."), GetLastError());

	delete wbuf;
	CloseHandle(ovlp.hEvent);
}

void do_readfile(HANDLE hfile, int pos, int size)
{
	char *rbuf = new char[size+1];
	memset(rbuf, 0, size+1);

	OVERLAPPED ovlp = {};
	ovlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ovlp.Offset = pos;

	PrnTs(_T("ReadFile: pos=%d bytes=%d"), pos, size);

	DWORD nBytesDone = 0;
	BOOL succ = ReadFile(hfile, rbuf, size, &nBytesDone, &ovlp);
	DWORD winerr = GetLastError();
	if(!succ && winerr==ERROR_IO_PENDING)
	{
		succ = GetOverlappedResult(hfile, &ovlp, &nBytesDone, TRUE);
	}

	if(succ) {
		PrnTs(_T("ReadFile success, got %d bytes:"), nBytesDone);
		printf("%s\n", rbuf);
	}
	else {
		PrnTs(_T("ReadFile fail, WinErr=%d."), GetLastError());
	}

	delete rbuf;
	CloseHandle(ovlp.hEvent);
}


void RWFile_steps(HANDLE hfile, TCHAR *argv[])
{
	int i;
	for(i=0; argv[i]!=nullptr; i++)
	{
		TCHAR *onecmd = argv[i];
		TCHAR *ctx = nullptr;

		TCHAR *wrpos = _tcstok_s(onecmd, _T(","), &ctx);
		TCHAR *wrsize = _tcstok_s(nullptr, _T(","), &ctx);
		TCHAR *wpattern = _tcstok_s(nullptr, _T(","), &ctx);

		if(wrpos[0]=='W' || wrpos[0]=='w')
		{
			printf("==== #%d write \n", i+1);

			int pos = _tcstoul(wrpos+1, nullptr, 0);
			int size = _tcstoul(wrsize, nullptr, 0);
			
			do_writefile(hfile, pos, size, wpattern);
		}
		else if(wrpos[0]=='R' || wrpos[0]=='r')
		{
			printf("==== #%d read \n", i+1);

			int pos = _tcstoul(wrpos+1, nullptr, 0);
			int size = _tcstoul(wrsize, nullptr, 0);
			
			do_readfile(hfile, pos, size);
		}
		else
		{
			PrnTs(_T("Wrong command token: %s"), onecmd);
			continue;
		}

		printf("==== Press Enter to execute next R/W command ====\n");
		wait_enter();
	}
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	if(argc==1)
	{
		_tprintf(_T("RWFileAtPos version %s.\n"), g_version);
		_tprintf(_T("Usage:\n"));
		_tprintf(_T("    RWFileAtPos <file> [Wpos1,size1] [Wpos2,size2] [Rpos3,size3] ...\n"));
		_tprintf(_T("Example:\n"));
		_tprintf(_T("    RWFileAtPos input.txt W4096,100,ab W8000,200,cd\n"));
		_tprintf(_T("    RWFileAtPos input.txt W4096,100,xx W8000,200,yy R4096,40\n"));

		exit(4);
	}

	const TCHAR *szfn = argv[1];

	// Scan through all params, to see whether we use read-only, write-only or read-write.
	DWORD DesiredAccess = 0;
	int i;
	for(i=2; argv[i]!=nullptr; i++)
	{
		TCHAR c = argv[i][0];
		if(c=='W' || c=='w')
			DesiredAccess |= GENERIC_WRITE;
		if(c=='R' || c=='r')
			DesiredAccess |= GENERIC_READ;
	}

	DWORD ShareMode = FILE_SHARE_READ|FILE_SHARE_WRITE;

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

	RWFile_steps(hfile, argv+2);

	PrnTs(_T("Calling Closehandle()."));
	DWORD succ = CloseHandle(hfile);
	if(succ)
		PrnTs(_T("Success Closehandle()."));
	else
		PrnTs(_T("CloseHandle() fail, WinErr=%d"), GetLastError());

	return 0;
}

