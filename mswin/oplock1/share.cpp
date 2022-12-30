#include "share.h"

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd)
{
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	buf[0]=_T('['); buf[1]=_T('\0'); buf[bufchars-1] = _T('\0');
	if(ymd) {
		_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%04d-%02d-%02d "), buf, 
			st.wYear, st.wMonth, st.wDay);
	}
	_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%02d:%02d:%02d.%03d]"), buf,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return buf;
}

void PrnTs(const TCHAR *fmt, ...)
{
	static int count = 0;
	static DWORD s_prev_msec = GetTickCount();

	DWORD now_msec = GetTickCount();

	TCHAR buf[1000] = {0};

	// Print timestamp to show that time has elapsed for more than one second.
	DWORD delta_msec = now_msec - s_prev_msec;
	if(delta_msec>=1000)
	{
		_tprintf(_T(".\n"));
	}

	TCHAR timebuf[40] = {};
	now_timestr(timebuf, ARRAYSIZE(timebuf));

	_sntprintf_s(buf, _TRUNCATE, _T("%s (+%3u.%03us) "), 
		timebuf, 
		delta_msec/1000, delta_msec%1000);

	int prefixlen = (int)_tcslen(buf);

	va_list args;
	va_start(args, fmt);

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);

	va_end(args);

	_tprintf(_T("%s\n"), buf);

	s_prev_msec = now_msec;
}

