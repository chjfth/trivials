/*------------------------------------------------------------
test_FlexiInfobox.cpp skeleton program 
-- Displays "Hello, WindowsX!" in client area
				 
   Compile it with command line in Visual C++ 2010+:
   
cl /c /Od /MT /Zi /D_DEBUG /D_UNICODE /DUNICODE test_FlexiInfobox.cpp
rc test_FlexiInfobox.rc
link /debug test_FlexiInfobox.obj test_FlexiInfobox.res kernel32.lib user32.lib gdi32.lib

   Then we can load it into Visual C++ debugger.
  ------------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "resource.h"

#define CHHI_ALL_IMPL
#include <mswin/utils_wingui.h>
#include <mswin/FlexiInfobox.h>

#pragma comment(lib, "comctl32.lib")

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


HINSTANCE g_hinst;

///////////////////////////////////////////////////////////////////////////////

struct DlgPrivate_st
{
	const WCHAR *mystr;
};

BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	util_SetWindowIcon(hwnd, MAKEINTRESOURCE(1));

	DlgPrivate_st *pr = new DlgPrivate_st;
	pr->mystr = (const WCHAR*)lParam;

	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)pr);

	SetDlgItemText(hwnd, IDC_EDIT_HINT, 
		_T("Hint: Press Shift to use NULL as hwndParent.\n")
		_T("Press Ctrl to show both OK/Cancel buttons.\n")
		);

	SetFocus(NULL);
	return(FALSE); // no default focus
}

void Dlg_OnDestroy(HWND hwnd)
{
	DlgPrivate_st *pr = (DlgPrivate_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	// Destroy all resources allocated back in Dlg_OnInitDialog().
	delete pr;
}

struct MyDsiContext_st
{
	bool isFromRC;
};

FibCallback_ret DSI_GetNowTime(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	(void)cb_info;
	MyDsiContext_st &ctx = *(MyDsiContext_st*)_ctx;

	SYSTEMTIME st;
	GetLocalTime(&st);
	snTprintf(textbuf, bufchars, 
		_T("%s\r\n\r\nNow time from GetLocalTime(): %04d-%02d-%02d %02d:%02d:%02d.%03d"), 
		ctx.isFromRC ? _T("Msgbox from RC") : _T("MsgBox from DLGITEMTEMPLATE"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
		);

	return FIBcb_OK;
}

void do_Flexi(HWND hwndParent, bool isUseRC, bool isMono)
{
	bool isShiftDown = GetKeyState(VK_SHIFT)<0 ? true:false;
	bool isCtrlDown = GetKeyState(VK_CONTROL)<0 ? true:false;

	HWND useParent = isShiftDown ? NULL : hwndParent;
		// Using NULL as hwndParent so that we can pop out one with RC and another without RC
		// for side-by-side comparison. 

	TCHAR szTimeText[400] = _T("to-modify");
	MyDsiContext_st ctx = { isUseRC ? true : false };

	FibInput_st si;
	si.title = _T("FlexiInfobox Sample");
	si.fixedwidth_font = isMono ? true : false;
	si.procGetText = DSI_GetNowTime;
	si.ctxGetText = &ctx;
	si.bufchars = ARRAYSIZE(szTimeText);
	si.msecAutoRefresh = 500;
	si.isAutoRefreshNow = true;
	si.msecDelayClose = 1000;
//	si.fontsize = 12;
	if(!isCtrlDown)
	{
//		si.idDefaultFocus = IDOK; // The button IDOK will defaulty have focus
		si.szBtnOK = _T("Cl&ose");
	}
	else
	{
		si.idDefaultFocus = IDCANCEL;
		si.szBtnOK = _T("&O K");
		si.szBtnCancel = _T("&Cancel");
	}
	// opt.isOnlyClosedByProgram = true;

	FIB_ret fibret;
	if(isUseRC)
	{
		fibret = ggt_FlexiInfobox_useRC(g_hinst, 
			MAKEINTRESOURCE(IDD_SHOW_INFO),
			useParent, &si, szTimeText); 
	}
	else
	{
		fibret = ggt_FlexiInfobox(useParent, &si, szTimeText);
	}

	TCHAR szret[80];
	snTprintf(szret, 
		_T("FIB_ret=%d\r\n")
		_T("1=OK/Yes 2=Cancel/No"), 
		fibret);
	SetDlgItemText(hwndParent, IDC_EDIT_HINT, szret);
}

void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{
//	DlgPrivate_st *pr = (DlgPrivate_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	void do_Cases(HWND);

	switch (id) 
	{{
	case IDC_USE_RC:
		do_Flexi(hwnd, true, false);
		break;
	case IDC_USE_RC_MONO:
		do_Flexi(hwnd, true, true);
		break;
	case IDC_NOT_USE_RC:
		do_Flexi(hwnd, false, false);
		break;
	case IDC_NOT_USE_RC_MONO:
		do_Flexi(hwnd, false, true);
		break;
	case IDC_BTN_CASES:
		do_Cases(hwnd);
		break;
	case IDOK:
	case IDCANCEL:
		EndDialog(hwnd, id);
		break;

	}}
}

void Dlg_OnLButtonDown(HWND hdlg, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	DlgPrivate_st *pr = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
}


INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hwnd, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hwnd, WM_DESTROY,       Dlg_OnDestroy);
		HANDLE_dlgMSG(hwnd, WM_COMMAND,       Dlg_OnCommand);
		HANDLE_dlgMSG(hwnd, WM_LBUTTONDOWN,   Dlg_OnLButtonDown);
		HANDLE_dlgMSG(hwnd, WM_LBUTTONDBLCLK, Dlg_OnLButtonDown);
	}
	return(FALSE);
}

int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) 
{
	InitCommonControls(); // WinXP requires this to see any v6 dialogbox.

	g_hinst = hinstExe;
	const WCHAR *mystr = L"My private string";
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)mystr);
	return(0);
}

