#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

typedef DWORD WinErr_t;

WinErr_t myCreateProcess(
	const TCHAR *pszExepath,
	TCHAR *szCommandLine, bool isShowWindow, bool isWait, 
	DWORD *p_subproc_exitcode)
{
	_tprintf(_T("Calling CreateProcess()...\n"));

	STARTUPINFO sti = {sizeof(STARTUPINFO)};
	sti.dwFlags = STARTF_USESHOWWINDOW;
	sti.wShowWindow = isShowWindow ? SW_SHOW : SW_HIDE;

	PROCESS_INFORMATION procinfo = {0};

	BOOL succ = CreateProcess(
		pszExepath,
		szCommandLine, 
		NULL, // process security attr
		NULL, // thread security attr
		FALSE, // bInheritHandles 
		isShowWindow ? 0 : CREATE_NO_WINDOW, // dwCreationFlags
		NULL, // lpEnvironment
		NULL, // lpCurrentDirectory  
		&sti, 
		&procinfo);
	if(!succ)
		return GetLastError();

	_tprintf(_T("CreateProcess() returned. Subprocess PID = %u\n"), procinfo.dwProcessId);

	if(isWait)
	{
		BOOL waitre = WaitForSingleObject(procinfo.hProcess, INFINITE);
		if(waitre==WAIT_OBJECT_0)
		{
			if(GetExitCodeProcess(procinfo.hProcess, p_subproc_exitcode)==FALSE)
			{
				_tprintf(_T("[PANIC!] GetExitCodeProcess() got WinErr=%d.\n"), GetLastError());
			}
		}
		else
		{
			_tprintf(_T("[PANIC!] WaitForSingleObjct() got WinErr=%d.\n"), GetLastError());
		}		
	}

	CloseHandle(procinfo.hThread);
	CloseHandle(procinfo.hProcess);

	return ERROR_SUCCESS;
}

const TCHAR *wincmdline_strip_argv0(
	const TCHAR *cmdline, const TCHAR *argv0, int *p_nspaces_after_argv0=0)
{
	// argv0 is definitely NOT surround by double-quotes.
	// cmdline may or may not be surround by double-quotes.
	//
	// Now checking cmdline, there may be some space-chars 
	// after argv0 part or after "argv0" part, we count the space-chars

	const TCHAR * argv0_in_cmdline = _tcsstr(cmdline, argv0);
	if(!argv0_in_cmdline)
	{
		_tprintf(_T("Unexpect! Cannot find argv[0] in cmdline.\n"));
		exit(4);
	}

	const TCHAR *pv0tail = argv0_in_cmdline + _tcslen(argv0);

	if(pv0tail[0]=='"')
	{
		pv0tail++;
	}

	if(pv0tail[0]=='\0')
	{
		if(p_nspaces_after_argv0)
			*p_nspaces_after_argv0 = 0;

		return nullptr;
	}

	// Now count space-chars
	int nspc = 0;
	while( pv0tail[nspc] == ' ' )
		nspc++;

	if(nspc==0)
	{
		_tprintf(_T("Unexpect! No space delimiter found after argv[0] part.\n"));
		exit(4);		
	}

	if(p_nspaces_after_argv0)
		*p_nspaces_after_argv0 = nspc;

	if(pv0tail[nspc]!='\0')
		return pv0tail + nspc;
	else
		return nullptr;
}

int _tmain(int argc, TCHAR* argv[])
{
	// Will use env-var SUBEXE_errormode's value as subproc's GetErrorMode() value.
	// Will use env-var SUBEXE's value as CreateProcess's first param(lpApplicationName).

	TCHAR szErrormode[16]={}; // you can try 1 (SEM_FAILCRITICALERRORS)
	GetEnvironmentVariable(_T("SUBEXE_errormode"), szErrormode, _countof(szErrormode));
	if(szErrormode[0])
	{
		UINT errormode = (UINT)_tcstoul(szErrormode, nullptr, 0);
		_tprintf(_T("Before CreateProcess, do SetErrorMode(%u);\n"), errormode);

		UINT orig = SetErrorMode(errormode);
		_tprintf(_T("SetErrorMode() returns %u\n"), orig);
	}
	else
	{
		_tprintf(_T("Env-var SUBEXE_errormode is empty, so will not call SetErrorMode().\n"));
	}

	TCHAR exepath[MAX_PATH]={};
	GetEnvironmentVariable(_T("SUBEXE"), exepath, MAX_PATH-1);
	if(exepath[0])
	{
		_tprintf(_T("Will call CreateProcess() with first param from env-var SUBEXE:\n"));
		_tprintf(_T("    %s\n"), exepath);
	}
	else
	{
		_tprintf(_T("Will call CreateProcess() with first param=NULL. (no env-var SUBEXE)\n"));
	}

	// Will use the whole string after argv[0] as CreateProcess's second param(lpCommandLine).
	// If empty content after argv[0], NULL is used as CreateProcess's second param.
	//
	// If you want to pass pns to lpCommandLine with pns[0]='\0', please write:
	//
	//	  subexe2 -

	const TCHAR *subexe_cmdline = wincmdline_strip_argv0(GetCommandLine(), argv[0]);
	TCHAR szCmdLine[MAX_PATH] = {};

	if(subexe_cmdline)
	{
		if(_tcscmp(subexe_cmdline, _T("-"))==0)
			szCmdLine[0] = '\0';
		else
			_tcscpy_s(szCmdLine, subexe_cmdline);

		_tprintf(_T("Will call CreateProcess() with second param: (%d chars)\n"), _tcslen(szCmdLine));
		_tprintf(_T("    %s\n"), szCmdLine);
	}
	else
	{
		_tprintf(_T("Will call CreateProcess() with second param=NULL.\n"));
	}

	DWORD subproc_exitcode = 444;

	WinErr_t winerr = myCreateProcess(
		exepath[0] ? exepath : NULL,
		szCmdLine, 
		true, 
		true, // is wait
		&subproc_exitcode
		); 
	// -- MSDN: szCmdLine[] may be modified inside; he injects temporal NUL after argv[0],
	//    then before CreateProcess() return, that user TCHAR is restored.
	//    So, MSDN requires the szCmdLine to be (non-const) TCHAR*. 

	Sleep(100);
	_tprintf(_T("\n"));

	if(winerr==0)
	{
		_tprintf(_T("[SUMUP] CreateProcess() succeeded. Sub-process exitcode=%u or %d (0x%X).\n"), 
			subproc_exitcode, subproc_exitcode, subproc_exitcode);
	}
	else
		_tprintf(_T("[SUMUP] CreateProcess() failed, WinErr=%d.\n"), winerr);

#if 0
	// This verifies that szCmdLine[] is "finally" unmodified after CreateProcess returns.
	if(winerr==0)
	{
		_tprintf(_T("Check post-CreateProcess() command-line string: (%d chars)\n"),
			_tcslen(szCmdLine));
		_tprintf(_T("    %s\n"), szCmdLine);
	}
#endif
	return 0;
}
