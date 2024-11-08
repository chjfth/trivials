#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <assert.h>
#include <stdio.h>

#include "utils.h"

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd)
{
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	buf[0]=_T('['); buf[1]=_T('\0'); buf[bufchars-1] = _T('\0');
	if(ymd) 
	{
		_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%04d-%02d-%02d "), buf, 
			st.wYear, st.wMonth, st.wDay);
	}
	_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%02d:%02d:%02d.%03d]"), buf,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return buf;
}

int vaMsgBox(HWND hwnd, UINT utype, const TCHAR *szTitle, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);

	TCHAR msgtext[4000] = {};
	_vsntprintf_s(msgtext, _TRUNCATE, szfmt, args);

	int ret = MessageBox(hwnd, msgtext, szTitle, utype);

	va_end(args);
	return ret;
}

void vaDbgTs(const TCHAR *fmt, ...)
{
	// Note: Each calling outputs one line, with timestamp prefix.
	// A '\n' will be added automatically at end.

	static int count = 0;
	static DWORD s_prev_msec = GetTickCount();

	DWORD now_msec = GetTickCount();

	TCHAR buf[1000] = {0};

	// Print timestamp to show that time has elapsed for more than one second.
	DWORD delta_msec = now_msec - s_prev_msec;
	if(delta_msec>=1000)
	{
		OutputDebugString(_T(".\n"));
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

	// add trailing \n
	int slen = (int)_tcslen(buf);
	if(slen==ARRAYSIZE(buf)-1)
		--slen;

	buf[slen] = '\n';
	buf[slen+1] = '\0';

	OutputDebugString(buf);

	s_prev_msec = now_msec;
}

void vaDbgS(const TCHAR *fmt, ...)
{
	// This only has Sequential prefix.

	static int count = 0;
	TCHAR buf[1000] = {0};

	_sntprintf_s(buf, ARRAYSIZE(buf)-3, _TRUNCATE, TEXT("[%d] "), ++count); // prefix seq

	int prefixlen = (int)_tcslen(buf);

	va_list args;
	va_start(args, fmt);

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);
	
	prefixlen = (int)_tcslen(buf);
	_tcsncpy_s(buf+prefixlen, 2, TEXT("\r\n"), _TRUNCATE); // add trailing \r\n

	va_end(args);

	// add trailing \n
	int slen = (int)_tcslen(buf);
	if(slen==ARRAYSIZE(buf)-1)
		--slen;

	buf[slen] = '\n';
	buf[slen+1] = '\0';

	OutputDebugString(buf);
}

void vaSetWindowText(HWND hwnd, const TCHAR *szfmt, ...)
{
	TCHAR tbuf[4000] = {};
	va_list args;
	va_start(args, szfmt);

	_vsntprintf_s(tbuf, _TRUNCATE, szfmt, args);

	SetWindowText(hwnd, tbuf);

	va_end(args);
}

void vaSetDlgItemText(HWND hwnd, int nIDDlgItem, const TCHAR *szfmt, ...)
{
	TCHAR tbuf[4000] = {};
	va_list args;
	va_start(args, szfmt);

	_vsntprintf_s(tbuf, _TRUNCATE, szfmt, args);

	SetDlgItemText(hwnd, nIDDlgItem, tbuf);

	va_end(args);
}

void vaAppendText_mled(HWND hedit, const TCHAR *szfmt, ...)
{
	TCHAR tbuf[4000] = {};
	va_list args;
	va_start(args, szfmt);

	_vsntprintf_s(tbuf, _TRUNCATE, szfmt, args);

	int pos = GetWindowTextLength (hedit);
	// -- [2024-10-26] Note: when pos reaches 30000, Edit_SetSel() will fail with
	// WinErr=5 (ERROR_ACCESS_DENIED).

	Edit_SetSel(hedit, pos, pos);
	Edit_ReplaceSel(hedit, tbuf);

	va_end(args);
}

//////////////////////////////////////////

typedef void PROC_WM_TIMER_call_once(void *usercontext);

struct WM_TIMER_call_once_st
{
	HWND hwnd;
	PROC_WM_TIMER_call_once *userproc;
	void *usercontext;
};

void CALLBACK WM_TIMER_call_once_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	WM_TIMER_call_once_st *indata = (WM_TIMER_call_once_st*)idEvent;

	indata->userproc(indata->usercontext);

	KillTimer(indata->hwnd, idEvent);
	delete indata;
}


bool WM_TIMER_call_once(HWND hwnd, int delay_millisec, 
	PROC_WM_TIMER_call_once *userproc, void *usercontext)
{
	// TODO: If hwnd==NULL, maybe we can create a hidden window automatically.
	assert(hwnd);
	if(!hwnd)
		return false;

	WM_TIMER_call_once_st *indata = new WM_TIMER_call_once_st;
	indata->hwnd = hwnd;
	indata->userproc = userproc;
	indata->usercontext = usercontext;

	UINT_PTR uret = SetTimer(hwnd, (UINT_PTR)indata, delay_millisec, WM_TIMER_call_once_TimerProc);
	if(uret!=(UINT_PTR)indata)
	{
		delete indata;
		return false;
	}

	return true;
}


