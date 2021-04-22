// wmprint.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void prn(const TCHAR *fmt=NULL, ...)
{
	if(fmt==NULL)
	{
		_tprintf(_T("\n"));
		return;
	}

	va_list args;
	va_start(args, fmt);

	_vtprintf(fmt, args);
	_tprintf(_T("\n"));

	va_end(args);
}


void DoPrintWindow(HWND hwnd, bool isClientAreaOnly)
{
	DWORD winerr = 0;
	RECT r = {};
	BOOL b = FALSE; 
	if(!isClientAreaOnly)
	{
		GetWindowRect(hwnd, &r);
	}
	else
	{
		GetClientRect(hwnd, &r);
	}

	int width = r.right - r.left;
	int height = r.bottom - r.top;


	HDC hdcSrc = GetDC(hwnd);
	HBITMAP hbmpDst = CreateCompatibleBitmap(hdcSrc, width, height);
	b = ReleaseDC(hwnd, hdcSrc);

	HDC hdcDst = CreateCompatibleDC(NULL);
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcDst, (HGDIOBJ)hbmpDst);

	b = PrintWindow(hwnd, hdcDst, 
		isClientAreaOnly ? PW_CLIENTONLY : 0
		);
	if(!b)
	{
		winerr = GetLastError();
		prn(_T("PrintWindow() fail with winerr=%d."), winerr);
		return;
	}

/*	// WM_PRINT cannot work across processes. 
	LRESULT lresult = SendMessage(hwnd, WM_PRINT, (WPARAM)hdcDst,
		PRF_CHILDREN | PRF_CLIENT | PRF_ERASEBKGND | PRF_NONCLIENT | PRF_OWNED
		);

*/
	SelectObject(hdcDst, hbmpOld);
	b = DeleteDC(hdcDst);

	b = OpenClipboard(hwnd);
	if(!b) {
		winerr = GetLastError();
		prn(_T("OpenClipboard() fail with winerr=%d"), winerr);
		return;
	}

	b = EmptyClipboard();
	HANDLE hclipdata = SetClipboardData(CF_BITMAP, hbmpDst);
	if(!hclipdata) {
		winerr = GetLastError();
		prn(_T("SetClipboardData(CF_BITMAP, ...) fail with winerr=%d"), winerr);
		return;
	}

	prn(_T("HWND image sent to clipboard. [%d , %d]"), width, height);

	b = CloseClipboard();

	b = DeleteObject(hbmpDst);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc==1)
	{
		prn(_T("seePrintWindow <HWND> [is_client_area_only]"));
		prn();
		prn(_T("Example: "));
		prn(_T("  seePrintWindow 005B14E2"));
		prn(_T("  seePrintWindow 005B14E2 1"));
		return 4;
	}
	const TCHAR *szhwnd = argv[1];

	const TCHAR *szClientArea = argc>2 ? argv[2] : _T("0");
	bool isClientArea = _tcscmp(szClientArea, _T("1"))==0 ? true : false;
	
	HWND hwnd = (HWND)_tcstoul(szhwnd, 0, 16);
	if(IsWindow(hwnd))
	{
		prn(_T("Input HWND 0x%08X is valid."), (UINT)hwnd);
	}
	else
	{
		prn(_T("Input HWND 0x%08X is NOT valid."), (UINT)hwnd);
		return 4;
	}

	RECT r = {};
	BOOL b = GetWindowRect(hwnd, &r);
	if(b)
	{
		prn(_T("  Left, Top    : %d,%d"), r.left, r.top);
		prn(_T("  Right,Bottom : %d,%d"), r.right, r.bottom);
		prn(_T(" [Width,Height] [%d , %d]"), r.right-r.left, r.bottom-r.top);
	}
	else
	{
		prn(_T("GetWindowRect() error. WinErr=%d"), GetLastError());
		return 4;
	}

	b = GetClientRect(hwnd, &r);
	if(b)
	{
		prn(_T(" Client Area :  [%d , %d]"), r.right-r.left, r.bottom-r.top);
	}
	else
	{
		prn(_T("GetClientRect() error. WinErr=%d"), GetLastError());
		return 4;
	}

	DoPrintWindow(hwnd, isClientArea);

	return 0;
}

