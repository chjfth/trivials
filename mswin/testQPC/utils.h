#pragma once

#include "targetver.h"

#include <tchar.h>

#define HANDLE_dlgMSG(hwnd, message, fn) \
  case (message): \
  return SetDlgMsgResult( hwnd, message, HANDLE_##message((hwnd), (wParam), (lParam), (fn)) );
  // For message processing in a WinAPI user's dialog-procedure, we need a further step
  // beyond that of windowsx.h's HANDLE_MSG(). This HANDLE_dlgMSG() applies that further step.
  // Ref: Raymond Chen https://devblogs.microsoft.com/oldnewthing/20031107-00/?p=41923

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false);

BOOL vaMsgBox(HWND hwnd, UINT utype, const TCHAR *szTitle, const TCHAR *szfmt, ...);
// -- utype: MB_OK|MB_ICONINFORMATION

BOOL vaSetWindowText(HWND hwnd, const TCHAR *szfmt, ...);
BOOL vaSetDlgItemText(HWND hwnd, int nIDDlgItem, const TCHAR *szfmt, ...);

void vaDbgTs(const TCHAR *fmt, ...);

void vaDbgS(const TCHAR *fmt, ...);

//////////////////////////////////////////////////////////////////////////

inline __int64 get_qpf()
{
	LARGE_INTEGER li = {};
	BOOL succ = QueryPerformanceFrequency(&li);
	if(!succ)
		return -1;
	else
		return li.QuadPart;
}

inline __int64 get_qpc()
{
	LARGE_INTEGER li = {};
	BOOL succ = QueryPerformanceCounter(&li);
	if(!succ)
		return -1;
	else
		return li.QuadPart;
}

inline TCHAR* BigNum64ToString(__int64 lNum, TCHAR szBuf[], int chBufSize)
{
	TCHAR szNum[100] = {};
	_sntprintf_s(szNum, _TRUNCATE, TEXT("%I64d"), lNum);
	NUMBERFMT nf = {};
	nf.NumDigits = 0;
	nf.LeadingZero = FALSE;
	nf.Grouping = 3;
	nf.lpDecimalSep = TEXT(".");
	nf.lpThousandSep = TEXT(",");
	nf.NegativeOrder = 0;
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, szNum, &nf, szBuf, chBufSize);
	return(szBuf);
}