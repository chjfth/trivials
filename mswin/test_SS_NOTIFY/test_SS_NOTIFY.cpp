#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include <mswin/WinUser.itc.h>
using namespace itc;

#include "iversion.h"

#include "utils.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDOK:
	case IDCANCEL:
	{
		EndDialog(hdlg, id);
		break;
	}
	}}
}


BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	SetDlgItemText(hdlg, IDC_EDIT1,
		_T("Hint: When hovering mouse over a SS_NOTIFY label, the dialog-box will not receive WM_MOUSEMOVE.\r\n")
		_T("See it yourself.")
		);
	
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

static void Show_MouseMessage(HWND hdlg, const TCHAR* whichmsg,
	int x, int y, UINT keyFlags)
{
	static int s_count = 0;
	s_count++;

	vaSetDlgItemText(hdlg, IDC_EDIT1, 
		_T("[#%d] %s: x=%d, y=%d\r\n")
		_T("keyFlags: %s")
		, 
		s_count, 
		whichmsg,
		x, y,
		ITCSv(keyFlags, MK_xxx_mouse));
}

void Dlg_OnMouseMove(HWND hdlg, int x, int y, UINT keyFlags)
{
	Show_MouseMessage(hdlg, _T("WM_MOUSEMOVE"), x, y, keyFlags);
}

void Dlg_OnLButtonDown(HWND hdlg, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	Show_MouseMessage(hdlg, _T("WM_LBUTTONDOWN"), x, y, keyFlags);
}

void Dlg_OnRButtonDown(HWND hdlg, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	Show_MouseMessage(hdlg, _T("WM_RBUTTONDOWN"), x, y, keyFlags);
}

void Dlg_OnLButtonUp(HWND hdlg, int x, int y, UINT keyFlags)
{
	Show_MouseMessage(hdlg, _T("WM_LBUTTONUP"), x, y, keyFlags);
}

void Dlg_OnRButtonUp(HWND hdlg, int x, int y, UINT keyFlags)
{
	Show_MouseMessage(hdlg, _T("WM_RBUTTONUP"), x, y, keyFlags);
}

INT_PTR WINAPI UserDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);

		HANDLE_dlgMSG(hdlg, WM_MOUSEMOVE,     Dlg_OnMouseMove);
		HANDLE_dlgMSG(hdlg, WM_LBUTTONDOWN,   Dlg_OnLButtonDown);
		HANDLE_dlgMSG(hdlg, WM_LBUTTONUP,   Dlg_OnLButtonUp);
		HANDLE_dlgMSG(hdlg, WM_RBUTTONDOWN,   Dlg_OnRButtonDown);
		HANDLE_dlgMSG(hdlg, WM_RBUTTONUP,   Dlg_OnRButtonUp);
	}
	return FALSE;
}



int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	DlgPrivate_st dlgdata = { _T("Hello.\r\nPrivate string here.") };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, (LPARAM)&dlgdata);

	return 0;
}
