/*------------------------------------------
About2-muires.cpp -- About Box Demo Program No. 2
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

TCHAR g_szUilang[100] = {};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR szCmdLine, int iCmdShow)
{
	// If run with parameter "0x0404", it will be passed to SetThreadUILanguage()
	// so that RC-resource(s) [with a different .rc-defined 'LANGUAGE'] will be loaded.
	
	LANGID uilang_from_param = (LANGID)strtoul(szCmdLine, NULL, 0);

	if(uilang_from_param==0)
	{
		LANGID truelangid = GetThreadUILanguage();
		_sntprintf_s(g_szUilang, _TRUNCATE, _T("GetThreadUILanguage() = 0x%04X"), truelangid);
	}
	else
	{
		LANGID truelangid = SetThreadUILanguage(uilang_from_param);
		if(truelangid==uilang_from_param)
		{
			_sntprintf_s(g_szUilang, _TRUNCATE, _T("SetThreadUILanguage(0x%04X) success."), truelangid);
		}
		else
		{
			DWORD winerr = GetLastError();
			_sntprintf_s(g_szUilang, _TRUNCATE, _T("SetThreadUILanguage(0x%04X) fail! WinErr=%d."), 
				truelangid, winerr);
		}
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

		LCID lcid_thread = GetThreadLocale();
		LCID lcid_ui = GetUserDefaultUILanguage();
		TCHAR lcname_thread[20] = {};
		TCHAR lcname_ui[20] = {};
		LCIDToLocaleName(lcid_thread, lcname_thread, ARRAYSIZE(lcname_thread), 0);
		LCIDToLocaleName(lcid_ui, lcname_ui, ARRAYSIZE(lcname_ui), 0);

		TCHAR szText[400]={};
		_sntprintf_s(szText, _TRUNCATE,
			_T("%s\n")
			_T("GetThreadLocale() = 0x%04X, %s\n")
			_T("GetUserDefaultUILanguage() = 0x%04X, %s")
			,
			g_szUilang,
			lcid_thread, lcname_thread,
			lcid_ui, lcname_ui);
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
