#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include "..\share.h"

int g_obufsize = 4000;
int g_ibufsize = 4000;
int g_defaulttimeout_msec = 1000;

void server_print_help()
{
	const TCHAR *pipename = _T("\\\\.\\pipe\\MyTestPipe");

	_tprintf(_T("Usage:\n"));
	_tprintf(_T("  PipeServer1 <pipe-name> <max-instances> [openmode-hex] [pipemode-hex]\n"));
	_tprintf(_T("\n"));
	_tprintf(_T("Examples:\n"));
	_tprintf(_T("  PipeServer1 %s 1\n"), pipename);
	_tprintf(_T("    -- Create a pipe-namespace & the only one pipe-instance.\n"));
	_tprintf(_T("  PipeServer1 %s 2 80003\n"), pipename);
	_tprintf(_T("    -- Create/Use a pipe-namespace and create one pipe-instance in that namespace.\n"));
	_tprintf(_T("       Max pipe-instances allow in that namespace is 2.\n"));
	_tprintf(_T("       dwOpenMode is 0x80003 (FILE_FLAG_FIRST_PIPE_INSTANCE | PIPE_ACCESS_DUPLEX)\n"));	
	_tprintf(_T("\n"));
	_tprintf(_T("If [openmode-hex] is not given, I'll use PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED.\n"));
	_tprintf(_T("If [pipemode-hex] is not given, I'll use 0.\n"));
	_tprintf(_T("Configurable by env-var:\n"));
	_tprintf(_T("  nOutBufferSize : Used by CreateNamedPipe()\n"));
	_tprintf(_T("   nInBufferSize : Used by CreateNamedPipe()\n"));
	_tprintf(_T("\n"));
}

void do_work(const TCHAR *pipename, int nmaxinstances, DWORD openmode, DWORD usemode)
{
	openmode |= FILE_FLAG_OVERLAPPED;

	TCHAR tbuf[100] = {};
	if(GetEnvironmentVariable(_T("nOutBufferSize"), tbuf, ARRAYSIZE(tbuf)) > 0)
		g_obufsize = _ttoi(tbuf);
	if(GetEnvironmentVariable(_T("nInBufferSize"), tbuf, ARRAYSIZE(tbuf)) > 0)
		g_ibufsize = _ttoi(tbuf);

	PrnTs(_T("Call CreateNamePipe()"));
	_tprintf(_T("  pipename       = \"%s\"\n"), pipename);
	_tprintf(_T("  max-instances  = %d\n"), nmaxinstances);
	_tprintf(_T("  dwOpenMode     = 0x%04X.%04X\n"), openmode>>16, openmode&0xFFFF);
	_tprintf(_T("  dwPipeMode     = 0x%04X.%04X\n"), usemode>>16, usemode&0xFFFF);
	_tprintf(_T("  nOutBufferSize = %d\n"), g_obufsize);
	_tprintf(_T("   nInBufferSize = %d\n"), g_ibufsize);

	BOOL succ = 0;
	DWORD winerr = 0;

	HANDLE hPipe = CreateNamedPipe(pipename, openmode, usemode, nmaxinstances, 
		g_obufsize, g_ibufsize, g_defaulttimeout_msec, NULL);

	if(hPipe==INVALID_HANDLE_VALUE)
	{
		winerr = GetLastError();
		_tprintf(_T("CreateNamedPipe() fail, winerr=%d.\n"), winerr);
		exit(3);
	}

	check_NamedPipeInfo(hPipe, ServerSide);

	OVERLAPPED ovlp = {};
	ovlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	PrnTs(_T("Calling ConnectNamedPipe()..."));
	succ = ConnectNamedPipe(hPipe, &ovlp);
	winerr = GetLastError();
	if(!succ && winerr==ERROR_IO_PENDING)
	{
		PrnTs(_T("  Async wait..."));

		//DWORD waitre = WaitForSingleObject(hPipe, INFINITE);
		DWORD nbret = 0;
		succ = GetOverlappedResult(hPipe, &ovlp, &nbret, TRUE);
	}

	winerr = GetLastError();
	if(!succ && winerr!=ERROR_PIPE_CONNECTED)
	{
		PrnTs(_T("ConnectNamedPipe() finally fails, winerr=%d"), winerr);
		exit(3);
	}

	PrnTs(_T("ConnectNamedPipe() success."), winerr);

	check_NamedPipeInfo(hPipe, ServerSide); // again, after ConnectNamedPipe() success

	do_interactive(hPipe);

	CloseHandle(hPipe);
	CloseHandle(ovlp.hEvent);
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	if(argc<3)
	{
		server_print_help();
		exit(4);
	}

	const TCHAR *pipename = argv[1];
	int nmax = _ttoi(argv[2]);

	if(nmax<1 || nmax>255)
	{
		_tprintf(_T("[ERROR] <max-instances> should be between 1-255\n"));
		exit(4);
	}

	DWORD openmode = PIPE_ACCESS_DUPLEX;
	if(argc>3)
	{
		openmode = _tcstoul(argv[3], NULL, 16);
	}

	DWORD usemode = PIPE_TYPE_BYTE | PIPE_WAIT; // that is 0
	if(argc>4)
	{
		usemode = _tcstoul(argv[4], NULL, 16);
	}

	do_work(pipename, nmax, openmode, usemode);

	return 0;
}

