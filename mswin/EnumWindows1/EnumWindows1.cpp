#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdarg.h>
#include <locale.h>

#include <mswin/winuser.itc.h>

struct WinEnumCallback_st
{
	HWND hwndParent;

	int NestLevel;
	int idxChildHwnd; 

	const TCHAR *pParentSeqs;
};

void PrnLv(int lv, const TCHAR *fmt, ...)
{
	// Print with indent level.
	// will add \n automatically

	TCHAR buf[2000] = {0};

	va_list args;
	va_start(args, fmt);

	int prefixlen = lv*4;
	_sntprintf_s(buf, _TRUNCATE, _T("%*s"), prefixlen, _T(""));

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-prefixlen-1, // -1 for trailing \n
		_TRUNCATE, fmt, args); 

	va_end(args);

	// add trailing \n
	int slen = (int)_tcslen(buf);
	buf[slen] = '\n';
	buf[slen+1] = '\0';

	_tprintf(_T("%s"), buf);
}

TCHAR * vaStrCat(TCHAR buf[], int bufsize, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int prefixlen = _tcslen(buf);
	
	_vsntprintf_s(buf+prefixlen, bufsize-prefixlen, _TRUNCATE, fmt, args);

	va_end(args);

	return buf;
}

BOOL CALLBACK PROC_GotWindow(HWND hwnd,	LPARAM cbextra)
{
	WinEnumCallback_st &cbe = *(WinEnumCallback_st*)cbextra;

	if(cbe.hwndParent)
	{
		// We will neglect deeper child-windows here. In other word:
		// EnumChildWindows() produce the whole window-tree beneath,
		// but we want to see only cbe.hwndParent's direct child.

		HWND parent = GetParent(hwnd);
		if(parent!=cbe.hwndParent)
		{
			return TRUE;
		}
	}

	TCHAR szwndtext[200] = {};
	GetWindowText(hwnd, szwndtext, ARRAYSIZE(szwndtext));

	TCHAR szSeq[100] = {};
	_sntprintf_s(szSeq, _TRUNCATE, 
		cbe.NestLevel==0 ? _T("%s%d") : _T("%s.%d")
		,
		cbe.NestLevel==0 ? _T("") : cbe.pParentSeqs,
		cbe.idxChildHwnd+1);

	PrnLv(cbe.NestLevel, _T("[%s] HWND=0x%08X \"%s\""), 
		szSeq, hwnd, szwndtext);

	UINT wStyle = GetWindowStyle(hwnd);

	// Recurse into child windows

	if(1)
	{
		WinEnumCallback_st cbe1 = {};
		cbe1.hwndParent = hwnd;
		cbe1.NestLevel = cbe.NestLevel + 1;
		cbe1.idxChildHwnd = 0;
		cbe1.pParentSeqs = szSeq;

		EnumChildWindows(hwnd, PROC_GotWindow, (LPARAM)&cbe1);
	}

	cbe.idxChildHwnd++;

	return TRUE;
}

void do_work()
{
	WinEnumCallback_st cbe = {};

	BOOL succ = EnumWindows(PROC_GotWindow, (LPARAM)&cbe);

}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	do_work();

	return 0;
}

