#pragma once

#ifdef UNICODE
#define Memset wmemset
#else
#define Memset memset
#endif

#define NULCHAR _T('\0')

#ifdef UNICODE
#define BADCHAR _T('\x203B') // the big asterisk. Don't use '\x2a2a' which causes debugger showing this WCHAR as a white box.
#else
#define BADCHAR _T('*')
#endif

#define TS_OK   _T("OK")
#define TS_SKIP _T("skip")
#define TS_BUG _T("[BUG]")
#define TS_WACKY_FILL   _T("[EdgeWackyFill]")  // Edge-case fill-style different than small-buf
#define TS_OVERFLOW_NUL _T("[OverflowNUL]")

#define TS_SMALL_BUFFER_OVERFLOW _T("[SMALLBUFFER-OVERFLOW]")// not found yet
#define TS_EDGE_BUFFER_OVERFLOW  _T("[EDGEBUFFER-OVERFLOW]") // not found yet

#define LEN_NO_REPORT (-2)  // some API does not report/feedback required buflen
#define FORGOT_INIT (-4)

#define STRLEN(s) (int)_tcslen(s)


void ReportTraits(const TCHAR *apiname,
	int small_buflen, int eret_size, DWORD winerr,
	const TCHAR *eoutput, 
	int sret_size, const TCHAR *soutput, 
	int edge_retsize=0, const TCHAR *edge_retbuf=NULL);

void check_apis();

void Prn(const TCHAR *fmt, ...);




inline const TCHAR *app_GetWindowsVersionStr3()
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
