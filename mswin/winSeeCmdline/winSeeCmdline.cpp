#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <assert.h>
#include <Shlwapi.h>

const TCHAR *g_szversion = _T("1.1");

const TCHAR *app_GetWindowsVersionStr3()
{
	typedef DWORD __stdcall PROC_RtlGetVersion(OSVERSIONINFOEX*);
	typedef BOOL __stdcall PROC_GetVersionEx(OSVERSIONINFOEX*);

	static TCHAR s_verstr[40];
	OSVERSIONINFOEX exver = { sizeof(OSVERSIONINFOEX) };

	PROC_RtlGetVersion *RtlGetVersion = (PROC_RtlGetVersion*)
		GetProcAddress(GetModuleHandle(_T("ntdll")), "RtlGetVersion");

	PROC_GetVersionEx *dllGetVersionEx = (PROC_GetVersionEx*)
		GetProcAddress(GetModuleHandle(_T("kernel32")), "GetVersionEx");

	if(RtlGetVersion)
		RtlGetVersion(&exver);

	if (exver.dwMajorVersion == 0)
	{
		// RtlGetVersion() fail, fall back to traditional GetVersionEx()
		BOOL succ = dllGetVersionEx && dllGetVersionEx(&exver);
		if (!succ)
			exver.dwMajorVersion = 0;
	}

	if (exver.dwMajorVersion > 0)
	{
		_sntprintf_s(s_verstr, ARRAYSIZE(s_verstr), _T("%d.%d.%d"),
			exver.dwMajorVersion, exver.dwMinorVersion, exver.dwBuildNumber);
	}
	else
	{
		_sntprintf_s(s_verstr, ARRAYSIZE(s_verstr), _T("%s"),
			_T("Fail to get Windows OS version after trying NTDLL!RtlGetVersion() and GetVersionEx()!"));
	}

	return s_verstr;
}

void app_print_version()
{
	_tprintf(_T("Program compiled at %s with _MSC_VER=%d (v%s)\n"), 
		_T(__DATE__), _MSC_VER, g_szversion);
	_tprintf(_T("This Windows OS version: %s\n"), app_GetWindowsVersionStr3());
	_tprintf(_T("\n"));
}

const TCHAR *wincmdline_strip_argv0(
	const TCHAR *cmdline, const TCHAR *argv0, int *p_nspaces_after_argv0=nullptr)
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
	setlocale(LC_ALL, "");

	app_print_version();

	const TCHAR *cmdline = GetCommandLine();
	int lenCmdline = (int)_tcslen(cmdline);
	_tprintf(_T("GetCommandLine() string length: %d\n"), lenCmdline);
	_tprintf(_T("\n"));

	_tprintf(_T("%s\n"), cmdline);

	int nspc = -1;
	const TCHAR *whole_cmdparams = wincmdline_strip_argv0(cmdline, argv[0], &nspc);
	_tprintf(_T("\n"));
	_tprintf(_T("Space-chars after argv0: %d\n"), nspc);
	if(whole_cmdparams)
		_tprintf(_T("Cmd-params  after argv0: %s\n"), whole_cmdparams);
	else
		_tprintf(_T("No cmd-params.\n"));

	const TCHAR *pArgStart = PathGetArgs(cmdline);
	if( (whole_cmdparams==nullptr && pArgStart[0]=='\0')
		||  _tcscmp(pArgStart, whole_cmdparams)==0 )
	{
		_tprintf(_T("OK. Same result from PathGetArgs().\n"));
	}
	else 
	{
		_tprintf(_T("PANIC!!! PathGetArgs() returns: %s\n"), pArgStart);
	}

	return 0;
}

