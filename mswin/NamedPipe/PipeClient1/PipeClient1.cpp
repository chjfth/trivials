#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include "..\share.h"

int g_startwait_msec = 0;
bool g_is_writethrough = false;
DWORD g_dwDesiredAccess = GENERIC_READ|GENERIC_WRITE;

void print_version()
{
	_tprintf(_T("PipeClient1 version: %s\n"), app_version);
}

void do_client(const TCHAR *pipename)
{
	g_whichside = ClientSide;

	load_envvars();

	DWORD winerr = 0;
	BOOL succ = 0;

	if(g_startwait_msec>0)
	{
		PrnTs(_T("Callling WaitNamedPipe()..."));
		succ = WaitNamedPipe(pipename, g_startwait_msec);
		if(succ)
			PrnTs(_T("WaitNamedPipe() success."));
		else if(winerr==WAIT_TIMEOUT)
			PrnTs(_T("WaitNamedPipe() timeout."));
		else
			PrnTs(_T("WaitNamedPipe() error, %s"), WinerrStr());
		
		if(!succ)
			return;
	}

	DWORD dwFlagsAndAttributes = FILE_FLAG_OVERLAPPED;
	dwFlagsAndAttributes |= g_is_writethrough ? FILE_FLAG_WRITE_THROUGH : 0;

	PrnTs(_T("Calling CreateFile()... \n")
		_T("  Pipename             = %s\n")
		_T("  dwDesiredAccess      = 0x%04X.%04X\n")
		_T("  dwFlagsAndAttributes = 0x%04X.%04X")
		,
		pipename,
		g_dwDesiredAccess>>16, g_dwDesiredAccess&0xFFFF,
		dwFlagsAndAttributes>>16, dwFlagsAndAttributes&0xFF
		);

	if(g_startwait_msec<=0)
	{
		_tprintf(_T("  (May block forever, Ctrl+c to break.)\n"));
	}

	HANDLE hPipe = CreateFile(pipename,
		g_dwDesiredAccess,
		0, // share mode
		NULL, // security attr
		OPEN_EXISTING,
		dwFlagsAndAttributes,
		NULL);
	if(hPipe==INVALID_HANDLE_VALUE)
	{
		PrnTs(_T("CreateFile() fail, %s"), WinerrStr());
		return;
	}
	else
		PrnTs(_T("CreateFile() success. Connected to pipe server."));

	check_NamedPipeInfo(hPipe);

	do_interactive(hPipe);

	PrnTs(_T("CloseHandle()..."));
	succ = CloseHandle(hPipe);
	PrnTs(_T("CloseHandle() done."));
	assert(succ);
}


void client_print_help()
{
	const TCHAR *pipename = _T("\\\\.\\pipe\\MyPipeSpace");

	_tprintf(_T("Usage:\n"));
	_tprintf(_T("  PipeClient1 <pipe-name> [wait=msec] [wthru] [dwDesiredAccess]\n"));
	_tprintf(_T("\n"));
	_tprintf(_T("  wait=msec        Call WaitNamedPipe() before actually connect.\n"));
	_tprintf(_T("  wthru            CreateFile() with FILE_FLAG_WRITE_THROUGH.\n"));
	_tprintf(_T("  dwDesiredAccess  Pass this specific value(in hex) to CreateFile()\n"));
	_tprintf(_T("                   default: GENERIC_READ|GENERIC_WRITE\n"));
	_tprintf(_T("\n"));
	_tprintf(_T("Examples:\n"));
	_tprintf(_T("  PipeClient1 %s\n"), pipename);
	_tprintf(_T("    -- Connect to a desired pipe-namespace.\n"));
	_tprintf(_T("  PipeClient1 %s wait=5000 wthru 0xC0040000\n"), pipename);
	_tprintf(_T("    -- Will first wait 5000 millisec in WaitNamedPipe().\n"));
	_tprintf(_T("       After WaitNamedPipe() success, CreateFile() w/ FILE_FLAG_WRITE_THROUGH,\n"));
	_tprintf(_T("       and dwDesiredAccess=GENERIC_READ|GENERIC_WRITE|WRITE_DAC .\n"));
	_tprintf(_T("\n"));
	_tprintf(_T("Configurable by env-var:\n"));
	_tprintf(_T("  WriteFileTimeout : Async-WriteFile() timeout millisec.\n"));
	_tprintf(_T("   ReadFileTimeout : Async- ReadFile() timeout millisec.\n"));
	_tprintf(_T("\n"));
}

bool ishexdigit(TCHAR c)
{
	if((c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F'))
		return true;
	else
		return false;
}

int _tmain(int argc, const TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	print_version();

	if(argc==1)
	{
		client_print_help();
		exit(4);
	}

	const TCHAR **argv_start = argv;

	const TCHAR *pipename = argv[1];
	
	for(argv=argv_start+2; *argv!=nullptr ;argv++)
	{
		const TCHAR *param = *argv;

		if( _tcsstr(param, _T("wait="))!=0 )
		{
			g_startwait_msec = _tcstoul(param+5, nullptr, 0);
		}
		else if( _tcscmp(param, _T("wthru"))==0 )
		{
			g_is_writethrough = true;
		}
		else if( ishexdigit(param[0]) )
		{
			g_dwDesiredAccess = _tcstoul(param, nullptr, 16);
		}
		else
		{
			_tprintf(_T("Invalid parameter: %s\n"), param);
			exit(4);
		}
	}

	do_client(pipename);

	return 0;
}

