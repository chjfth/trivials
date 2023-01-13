/*------------------------------------------
About2-muires.cpp
-- based on About Box Demo Program No.2 from [PRWIN5]
------------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK AboutDlgProc(HWND, UINT, WPARAM, LPARAM);

int iCurrentColor = IDC_BLACK,
iCurrentFigure = IDC_RECT;

static TCHAR szAppName[] = TEXT("About2-muires");

TCHAR g_szThreadUilang[100] = {};

typedef int WINAPI FUNCTYPE_LCIDToLocaleName(LCID Locale, LPTSTR lpName, int cchName, DWORD dwFlags);
FUNCTYPE_LCIDToLocaleName *dlptr_LCIDToLocaleName = 
	(FUNCTYPE_LCIDToLocaleName*)GetProcAddress(
		LoadLibrary(_T("KERNEL32.DLL")), "LCIDToLocaleName");

typedef LANGID WINAPI FUNCTYPE_GetThreadUILanguage();
FUNCTYPE_GetThreadUILanguage *dlptr_GetThreadUILanguage = 
	(FUNCTYPE_GetThreadUILanguage*)GetProcAddress(
		LoadLibrary(_T("KERNEL32.DLL")), "GetThreadUILanguage"); // WinXP does NOT have this

typedef LANGID WINAPI FUNCTYPE_SetThreadUILanguage(LANGID);
FUNCTYPE_SetThreadUILanguage *dlptr_SetThreadUILanguage = 
	(FUNCTYPE_SetThreadUILanguage*)GetProcAddress(
		LoadLibrary(_T("KERNEL32.DLL")), "SetThreadUILanguage"); // actually, WinXP has this


void MyChangeThreadUILang(PSTR szCmdLine)
{
	LANGID uilang_from_param = (LANGID)strtoul(szCmdLine, NULL, 0);

	if(uilang_from_param==0)
	{
		if(dlptr_GetThreadUILanguage)
		{	
			LANGID truelangid = dlptr_GetThreadUILanguage();
			_sntprintf_s(g_szThreadUilang, _TRUNCATE, _T("GetThreadUILanguage() = 0x%04X"), truelangid);
		}
	}
	else
	{
		LANGID truelangid = dlptr_SetThreadUILanguage(uilang_from_param);
		if(truelangid==uilang_from_param)
		{
			_sntprintf_s(g_szThreadUilang, _TRUNCATE, _T("SetThreadUILanguage(0x%04X) success."), uilang_from_param);
		}
		else
		{
			DWORD winerr = GetLastError();
			_sntprintf_s(g_szThreadUilang, _TRUNCATE, _T("SetThreadUILanguage(0x%04X)=0x%04X. WinErr=%d."), 
				uilang_from_param, truelangid, winerr);
		}
	}	
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	if(dlptr_SetThreadUILanguage)
	{
		// If run with parameter "0x0404", it will be passed to SetThreadUILanguage()
		// so that RC-resource(s) [with a different .rc-defined 'LANGUAGE'] will be loaded.
		//
		MyChangeThreadUILang(szCmdLine);
	}
	
	DialogBox(hInstance, TEXT("AboutBox"), NULL, AboutDlgProc);

	return 0;
}


void PaintWindow(HWND hwnd, int iColor, int iFigure)
{
	// Chj: `hwnd` can be the program's main window, or a control.

	static COLORREF crColor[8] = { RGB(0,   0, 0), RGB(0,   0, 255),
		RGB(0, 255, 0), RGB(0, 255, 255),
		RGB(255,   0, 0), RGB(255,   0, 255),
		RGB(255, 255, 0), RGB(255, 255, 255) };

	HBRUSH          hBrush;
	HDC             hdc;
	RECT            rect;

	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rect);
	hBrush = CreateSolidBrush(crColor[iColor - IDC_BLACK]);
	hBrush = (HBRUSH)SelectObject(hdc, hBrush);

	if (iFigure == IDC_RECT)
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	else
		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);

	DeleteObject(SelectObject(hdc, hBrush));
	ReleaseDC(hwnd, hdc);
}

void PaintTheBlock(HWND hCtrl, int iColor, int iFigure)
{
	InvalidateRect(hCtrl, NULL, TRUE);
	UpdateWindow(hCtrl);
	PaintWindow(hCtrl, iColor, iFigure);
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	static HWND hCtrlBlock;
	static int  iColor, iFigure;

	switch (message)
	{{
	case WM_INITDIALOG:
	{
		SetWindowText(hDlg, szAppName);
			
		iColor = iCurrentColor;
		iFigure = iCurrentFigure;

		CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, iColor);
		CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, iFigure);

		hCtrlBlock = GetDlgItem(hDlg, IDC_PAINT);

		LCID lcid_cplUILang = GetUserDefaultUILanguage();
		TCHAR lcname_cplUILang[20] = {}; // cpl: the setting from control-panel
		LCID lcid_threadloc = GetThreadLocale();
		TCHAR lcname_threadloc[20] = {};

		if(dlptr_LCIDToLocaleName)
		{
			// Vista+ has LCIDToLocaleName()
			dlptr_LCIDToLocaleName(lcid_cplUILang, lcname_cplUILang, ARRAYSIZE(lcname_cplUILang), 0);
			dlptr_LCIDToLocaleName(lcid_threadloc, lcname_threadloc, ARRAYSIZE(lcname_threadloc), 0);
		}
			
		TCHAR szText[400]={};
		_sntprintf_s(szText, _TRUNCATE,
			_T("%s\n")
			_T("GetUserDefaultUILanguage() = 0x%04X, %s\n")
			_T("GetThreadLocale() = 0x%04X, %s")
			,
			g_szThreadUilang,
			lcid_cplUILang, lcname_cplUILang,
			lcid_threadloc, lcname_threadloc);
		SetDlgItemText(hDlg, IDC_STATIC_LOCALE, szText);

		SetFocus(GetDlgItem(hDlg, iColor));
		return FALSE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			iCurrentColor = iColor;
			iCurrentFigure = iFigure;
			EndDialog(hDlg, TRUE);
			return TRUE;

		case IDCANCEL:
			EndDialog(hDlg, FALSE);
			return TRUE;

		case IDC_BLACK:
		case IDC_RED:
		case IDC_GREEN:
		case IDC_YELLOW:
		case IDC_BLUE:
		case IDC_MAGENTA:
		case IDC_CYAN:
		case IDC_WHITE:
			iColor = LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_BLACK, IDC_WHITE, LOWORD(wParam));
			PaintTheBlock(hCtrlBlock, iColor, iFigure);
			return TRUE;

		case IDC_RECT:
		case IDC_ELLIPSE:
			iFigure = LOWORD(wParam);
			CheckRadioButton(hDlg, IDC_RECT, IDC_ELLIPSE, LOWORD(wParam));
			PaintTheBlock(hCtrlBlock, iColor, iFigure);
			return TRUE;
		}
		break;

	case WM_PAINT:
		PaintTheBlock(hCtrlBlock, iColor, iFigure);
		break;
	}}
	return FALSE;
}
