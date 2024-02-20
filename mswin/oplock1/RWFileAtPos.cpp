#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <conio.h>
#include <windows.h>
#include "share.h"

const TCHAR *g_version = _T("1.6");

void wait_enter()
{
	while(_getch()!='\r');
}

void do_writefile(HANDLE hfile, int pos, int bytelen_totwrite, const TCHAR *wpattern)
{
	char wbpattern[100] = {};
	const char* pbytes_pattern = wbpattern;

#ifdef UNICODE
	WideCharToMultiByte(CP_ACP, 0, wpattern,-1, wbpattern, sizeof(wbpattern), NULL, NULL);
#else
	pbytes_pattern = wpattern;
#endif

	int bytelen_pattern = (int)strlen(pbytes_pattern);
	char *writebuf = new char[bytelen_totwrite + bytelen_pattern];

	int j=0;
	for(; j<bytelen_totwrite; j+=bytelen_pattern)
	{
		memcpy(writebuf+j, pbytes_pattern, bytelen_pattern);
	}
	writebuf[j] = '\0'; // for debug ease

	OVERLAPPED ovlp = {};
	ovlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ovlp.Offset = pos;

	PrnTs(_T("WriteFile: pos=%d bytes=%d pattern=%s"), pos, bytelen_totwrite, wpattern);

	DWORD nBytesDone = 0;
	BOOL succ = WriteFile(hfile, writebuf, bytelen_totwrite, &nBytesDone, &ovlp);
	DWORD winerr = GetLastError();
	if(!succ && winerr==ERROR_IO_PENDING)
	{
		succ = GetOverlappedResult(hfile, &ovlp, &nBytesDone, TRUE);
	}

	if(succ)
		PrnTs(_T("WriteFile success, written %d bytes."), nBytesDone);
	else
		PrnTs(_T("WriteFile fail, WinErr=%d."), GetLastError());

	delete writebuf;
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

		bool cmdok = false; // assume false

		if(wrpos && wrsize)
		{
			if(wrpos[0]=='R' || wrpos[0]=='r')
			{
				printf("==== #%d read \n", i+1);

				int pos = _tcstoul(wrpos+1, nullptr, 0);
				int size = _tcstoul(wrsize, nullptr, 0);

				do_readfile(hfile, pos, size);

				cmdok = true;
			}
			else if((wrpos[0]=='W' || wrpos[0]=='w') && wpattern)
			{
				printf("==== #%d write \n", i+1);

				int pos = _tcstoul(wrpos+1, nullptr, 0);
				int size = _tcstoul(wrsize, nullptr, 0);

				do_writefile(hfile, pos, size, wpattern);

				cmdok = true;
			}
		}

		if(!cmdok)
		{
			PrnTs(_T("[ERROR] Wrong command token: %s"), onecmd);
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
		_tprintf(_T("RWFileAtPos version %s\n"), g_version);
		_tprintf(_T("Usage:\n"));
		_tprintf(_T("    RWFileAtPos <file> [S?] [Wpos1,size1] [Wpos2,size2] [Rpos3,size3] ...\n"));
		_tprintf(_T("S? can be:\n"));
		_tprintf(_T("    S1: FILE_SHARE_READ, S2: FILE_SHARE_WRITE, S3: both, default: 0\n"));
		_tprintf(_T("    Use '+' before 'S' to force create a new file(clear old file).\n"));
		_tprintf(_T("Example:\n"));
		_tprintf(_T("    RWFileAtPos input.txt     W4096,100,ab  W8000,200,cd\n"));
		_tprintf(_T("    RWFileAtPos input.txt     W4096,100,xx  W8000,200,yy  R4096,40\n"));
		_tprintf(_T("    RWFileAtPos input.txt  S2 R4096,10\n"));
		_tprintf(_T("    RWFileAtPos input.txt +S2 W4096,10,abcd\n"));

		exit(4);
	}

	const TCHAR *szfn = argv[1];
	DWORD ShareMode = 0;
	int idxRW = 2;
	bool isCreateNewFile = false;

	if(argc>2)
	{
		TCHAR *pc = argv[2];

		if(*pc=='+')
		{
			isCreateNewFile = true;
			pc++;

			idxRW = 3;
		}

		if(*pc=='S' || *pc=='s')
		{
			TCHAR schar = pc[1];
			if(schar=='1')
				ShareMode = FILE_SHARE_READ;
			else if(schar=='2')
				ShareMode = FILE_SHARE_WRITE;
			else if(schar='3')
				ShareMode = FILE_SHARE_READ|FILE_SHARE_WRITE;

			idxRW = 3;
		}
	}

	// Scan through remaining params, to see whether we use read-only, write-only or read-write.
	DWORD DesiredAccess = 0;
	int i;
	for(i=idxRW; argv[i]!=nullptr; i++)
	{
		TCHAR c = argv[i][0];
		if(c=='W' || c=='w')
			DesiredAccess |= GENERIC_WRITE;
		if(c=='R' || c=='r')
			DesiredAccess |= GENERIC_READ;
	}

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
		isCreateNewFile ? CREATE_ALWAYS : OPEN_EXISTING, // dwCreationDisposition
		FILE_FLAG_OVERLAPPED,
		NULL);

	DWORD winerr = GetLastError();
	if(hfile==INVALID_HANDLE_VALUE) {
		PrnTs(_T("File open error. WinErr=%d."), winerr);
		exit(4);
	}

	const TCHAR *pszResult = _T("");
	if(isCreateNewFile)
	{
		pszResult = (winerr==ERROR_ALREADY_EXISTS ? 
			_T("(old file cleared)") : _T("(created new file)") );
	}

	PrnTs(_T("CreateFile() success, hfile=0x%p. %s"), hfile, pszResult);

	RWFile_steps(hfile, argv+idxRW);

	PrnTs(_T("Calling Closehandle()."));
	DWORD succ = CloseHandle(hfile);
	if(succ)
		PrnTs(_T("Success Closehandle()."));
	else
		PrnTs(_T("CloseHandle() fail, WinErr=%d"), GetLastError());

	return 0;
}
