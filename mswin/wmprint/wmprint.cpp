// wmprint.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

void prn(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	_vtprintf(fmt, args);

	va_end(args);
}


void DoPrintWindow(HWND hwnd)
{
	DWORD winerr = 0;
	RECT r = {};
	BOOL b = GetWindowRect(hwnd, &r);
	int width = r.right - r.left;
	int height = r.bottom - r.top;

	HDC hdcSrc = GetDC(hwnd);
	HBITMAP hbmpDst = CreateCompatibleBitmap(hdcSrc, width, height);
	b = ReleaseDC(hwnd, hdcSrc);

	HDC hdcDst = CreateCompatibleDC(NULL);
	HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcDst, (HGDIOBJ)hbmpDst);

	b = PrintWindow(hwnd, hdcDst, 0);
	if(!b)
	{
		winerr = GetLastError();
		prn(_T("PrintWindow() fail with winerr=%d.\n"), winerr);
	}

/*	// WM_PRINT cannot work across processes. 
	LRESULT lresult = SendMessage(hwnd, WM_PRINT, (WPARAM)hdcDst,
		PRF_CHILDREN | PRF_CLIENT | PRF_ERASEBKGND | PRF_NONCLIENT | PRF_OWNED
		);

*/
	SelectObject(hdcDst, hbmpOld);
	b = DeleteDC(hdcDst);

	b = OpenClipboard(hwnd);
	b = EmptyClipboard();
	HANDLE hclipdata = SetClipboardData(CF_BITMAP, hbmpDst);
	b = CloseClipboard();

	b = DeleteObject(hbmpDst);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc==1)
	{
		_tprintf(_T("Need a HWND value(hex) as parameter.\n"));
		return 4;
	}

	HWND hwnd = (HWND)_tcstoul(argv[1], 0, 16);
	if(IsWindow(hwnd))
	{
		_tprintf(_T("Input HWND 0x%08X is valid.\n"), (UINT)hwnd);
	}
	else
	{
		_tprintf(_T("Input HWND 0x%08X is NOT valid.\n"), (UINT)hwnd);
		return 4;
	}

	RECT r = {};
	BOOL b = GetWindowRect(hwnd, &r);
	if(b)
	{
		_tprintf(_T("  Left, Top    : %d,%d\n"), r.left, r.top);
		_tprintf(_T("  Right,Bottom : %d,%d\n"), r.right, r.bottom);
		_tprintf(_T(" [Width,Height] [%d , %d]\n"), r.right-r.left, r.bottom-r.top);
	}
	else
	{
		_tprintf(_T("GetWindowRect() error. WinErr=%d\n"), GetLastError());
	}

	DoPrintWindow(hwnd);	

	return 0;
}

