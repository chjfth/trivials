#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdarg.h>
#include <locale.h>

#include <mswin/win32cozy.h>

#include <mswin/winuser.itc.h>
#include <mswin/commctrl.itc.h>

struct WinEnumCallback_st
{
	HWND hwndParent;

	int NestLevel;
	int idxChildHwnd; 

	const TCHAR *pParentSeqs;
};

void replace_crlf(TCHAR *s)
{
	while(*s)
	{
		if(*s=='\r' || *s=='\n')
			*s = '/';
		s++;
	}
}

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

	int prefixlen = (int)_tcslen(buf);
	
	_vsntprintf_s(buf+prefixlen, bufsize-prefixlen, _TRUNCATE, fmt, args);

	va_end(args);

	return buf;
}

void myPrnWndInfo(int prnlv, HWND hwnd, const TCHAR *wndclass)
{
	UINT wStyle = GetWindowStyle(hwnd);
	UINT wexStyle = GetWindowExStyle(hwnd);

	if(! (wStyle & WS_CHILD) )
	{
		// If a toplevel window:
		if( _tcsicmp(wndclass, _T("tooltips_class32"))==0 )
			PrnLv(prnlv, _T("Style  : %s"), ITCSv(wStyle, itc::WS_xxx_tooltips_class32_toplevel));
		else
			PrnLv(prnlv, _T("Style  : %s"), ITCSv(wStyle, itc::WS_xxx_toplevel));
	}
	else if( _tcsicmp(wndclass, _T("static"))==0 )
	{
		PrnLv(prnlv, _T("Style  : %s"), ITCSv(wStyle, itc::WS_xxx_Static));
	}
	else if( _tcsicmp(wndclass, _T("edit"))==0 )
	{
		PrnLv(prnlv, _T("Style  : %s"), ITCSv(wStyle, itc::WS_xxx_Edit));
	}
	else if( _tcsicmp(wndclass, _T("button"))==0 )
	{
		PrnLv(prnlv, _T("Style  : %s"), ITCSv(wStyle, itc::WS_xxx_Button));
	}
	else if( _tcsicmp(wndclass, _T("listbox"))==0 )
	{
		PrnLv(prnlv, _T("Style  : %s"), ITCSv(wStyle, itc::WS_xxx_ListBox));
	}
	else if( _tcsicmp(wndclass, _T("combobox"))==0 )
	{
		PrnLv(prnlv, _T("Style  : %s"), ITCSv(wStyle, itc::WS_xxx_ComboBox));
	}
	else if( _tcsicmp(wndclass, _T("SysListView32"))==0 )
	{
		UINT eexstyle = ListView_GetExtendedListViewStyle(hwnd);
		PrnLv(prnlv, _T("Style  : %s"), ITCSv(wStyle, itc::WS_xxx_SysListView32));
		PrnLv(prnlv, _T("LVS_EX_: %s"), ITCSv(eexstyle, itc::LVS_EX_xxx));
	}
	else if( _tcsicmp(wndclass, _T("SysTreeView32"))==0 )
	{
		UINT eexstyle = TreeView_GetExtendedStyle(hwnd);
		PrnLv(prnlv, _T("Style  : %s"), ITCSv(wStyle, itc::WS_xxx_SysTreeView32));
		PrnLv(prnlv, _T("TVS_EX_: %s"), ITCSv(eexstyle, itc::TVS_EX_xxx));
	}
	else if( _tcsicmp(wndclass, _T("tooltips_class32"))==0 )
	{
		PrnLv(prnlv, _T("Style  : %s"), ITCSv(wStyle, itc::WS_xxx_tooltips_class32_childwnd));
	}
	else
	{	// other child-window class
		PrnLv(prnlv, _T("Style  : %s"), ITCSv(wStyle, itc::WS_xxx_childwnd));
	}

	if(wexStyle)
	{
		PrnLv(prnlv, _T("ExStyle: %s"), ITCSv(wexStyle, itc::WS_EX_xxx));
	}
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

	TCHAR szwndtext[200] = _T("(unknown window text)");
	szwndtext[ARRAYSIZE(szwndtext)-1] = '\0';
	GetWindowText(hwnd, szwndtext, ARRAYSIZE(szwndtext)-1);
	replace_crlf(szwndtext);

	TCHAR wndclass[100] = {};
	GetClassName(hwnd, wndclass, ARRAYSIZE(wndclass));

	TCHAR szSeq[100] = {};
	_sntprintf_s(szSeq, _TRUNCATE, 
		cbe.NestLevel==0 ? _T("%s%d") : _T("%s.%d")
		,
		cbe.NestLevel==0 ? _T("") : cbe.pParentSeqs,
		cbe.idxChildHwnd+1);

	RECT rect = {-1, -1, -1, -1};
	GetWindowRect(hwnd, &rect);

	TCHAR rtext[60] = {};
	PrnLv(cbe.NestLevel, _T("[%s] HWND=0x%08X <%s> \"%s\" Rect:%s" ), 
		szSeq, hwnd,  wndclass, szwndtext,
		RECTtext(rect, rtext, ARRAYSIZE(rtext))
		);

	// Print window "class" and style info
	myPrnWndInfo(cbe.NestLevel+1, hwnd, wndclass);

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

	_tprintf(_T("EnumWindows1.exe v1.1\n"));

	auto s = itc::WS_xxx_Button.Interpret(0x50000000, itc::DF_NameAndValue);

	do_work();

	return 0;
}

