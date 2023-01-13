#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include "..\share.h"

void print_version()
{
	_tprintf(_T("PipeServer1 version: %s\n"), app_version);
}

void do_server(const TCHAR *pipename, int nmaxinstances, DWORD openmode, DWORD usemode)
{
	g_whichside = ServerSide;

	load_envvars();

	openmode |= FILE_FLAG_OVERLAPPED;

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
		g_obufsize, g_ibufsize, g_timeout_client_prewait, NULL);

	if(hPipe==INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("CreateNamedPipe() fail, %s.\n"), WinerrStr());
		exit(3);
	}

//	check_NamedPipeInfo(hPipe, ServerSide);

	if(g_delaymsec_bfr_accept>0)
	{
		PrnTs(_T("Delay %d millisec before accepting client..."), g_delaymsec_bfr_accept);
		Sleep(g_delaymsec_bfr_accept);
	}


	OVERLAPPED ovlp = {};
	ovlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	if(g_is_skip_ConnectNamedPipe==1)
	{
		PrnTs(_T("==== SKIP ConnectNamedPipe(). ===="));
	}
	else
	{
		succ = do_ConnectNamedPipe(hPipe, ovlp);
	}

	show_NamedPipeInfo(hPipe);

	do_interactive(hPipe);

	PrnTs(_T("Calling CloseHandle()..."));
	succ = CloseHandle(hPipe);
	PrnTs(_T("Done    CloseHandle() ."));
	assert(succ);

	CloseHandle(ovlp.hEvent);
}

void server_print_help()
{
	const TCHAR *pipename = _T("\\\\.\\pipe\\MyPipeSpace");

	_tprintf(_T("Usage:\n"));
	_tprintf(_T("  PipeServer1 <pipe-name> <max-instances> [openmode-hex] [pipemode-hex]\n"));
	_tprintf(_T("\n"));
	_tprintf(_T("Examples:\n"));
	_tprintf(_T("  PipeServer1 %s 1\n"), pipename);
	_tprintf(_T("    -- Create a pipe-namespace & the only one pipe-instance.\n"));
	_tprintf(_T("  PipeServer1 %s 2 0x80003\n"), pipename);
	_tprintf(_T("    -- Create/Use a pipe-namespace and create one pipe-instance in that namespace.\n"));
	_tprintf(_T("       Max pipe-instances allow in that namespace is 2.\n"));
	_tprintf(_T("       dwOpenMode is 0x80003 (FILE_FLAG_FIRST_PIPE_INSTANCE | PIPE_ACCESS_DUPLEX)\n"));	
	_tprintf(_T("\n"));
	_tprintf(_T("If [openmode-hex] is not given, I'll use PIPE_ACCESS_DUPLEX|FILE_FLAG_OVERLAPPED.\n"));
	_tprintf(_T("If [pipemode-hex] is not given, I'll use 0.\n"));
	_tprintf(_T("Configurable by env-var:\n"));
	_tprintf(_T("  nOutBufferSize   : Used by CreateNamedPipe()\n"));
	_tprintf(_T("   nInBufferSize   : Used by CreateNamedPipe()\n"));
	_tprintf(_T("  WriteFileTimeout : Async-WriteFile() timeout millisec.\n"));
	_tprintf(_T("   ReadFileTimeout : Async- ReadFile() timeout millisec.\n"));
	_tprintf(_T("  DelayAcceptMsec  : Delay millisec between CreateNamedPipe and ConnectNamedPipe.\n"));
	_tprintf(_T("  SkipConnectNamedPipe=1 : Skip the unimportant ConnectNamedPipe().\n"));
	_tprintf(_T("\n"));
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	print_version();
	
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

	do_server(pipename, nmax, openmode, usemode);

	return 0;
}

