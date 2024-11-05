#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "..\utils.h"

#define JULAYOUT_IMPL
#include "..\JULayout2.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

void Execute_ShowWindow(HWND hdlg)
{
	static int s_count = 0;
	s_count++;

	const int Bufsize = 50;
	TCHAR tbuf[Bufsize] = {};

	GetDlgItemText(hdlg, IDC_EDIT_HWND, tbuf, Bufsize-1);
	HWND hwndTgt = (HWND)_tcstoul(tbuf, NULL, 16);

	GetDlgItemText(hdlg, IDC_nCmdShow, tbuf, Bufsize-1);
	int nCmdShow = _ttoi(tbuf);

	if(!IsWindow(hwndTgt))
	{
		vaSetDlgItemText(hdlg, IDC_EDIT_INFO, _T("0x%08X is not a HWND."), (UINT)hwndTgt);
		return;
	}

	BOOL isPrevVis = ShowWindow(hwndTgt, nCmdShow);
	vaSetDlgItemText(hdlg, IDC_EDIT_INFO, 
		_T("[#%d] ShowWindow(0x%08X, %s) returns %d\r\n\r\n0x%08X %s."), 
		s_count,
		(UINT)hwndTgt, tbuf, isPrevVis, 
		(UINT)hwndTgt, isPrevVis?_T("was Visible"):_T("was Hidden"));
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDC_BTN_EXECUTE:
	{
		Execute_ShowWindow(hdlg);
		break;
	}
	case IDOK:
	case IDCANCEL:
	{
		EndDialog(hdlg, id);
		break;
	}
	}}
}

// Sets the dialog box icons
inline void chSETDLGICONS(HWND hwnd, int idi) {
	SendMessage(hwnd, WM_SETICON, TRUE,  (LPARAM)
		LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		MAKEINTRESOURCE(idi)));
	SendMessage(hwnd, WM_SETICON, FALSE, (LPARAM)
		LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		MAKEINTRESOURCE(idi)));
}

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

//	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
//	jul->AnchorControl(0,0, 100,100, IDC_EDIT1);
	jul->AnchorControl(50,100, 50,100, IDC_BTN_EXECUTE);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

void Fill_DropdownList(HWND hdlg)
{
	HWND hctl = GetDlgItem(hdlg, IDC_nCmdShow);
	ComboBox_AddString(hctl, _T("0 SW_HIDE"));
	ComboBox_AddString(hctl, _T("1 SW_SHOWNORMAL"));
	ComboBox_AddString(hctl, _T("2 SW_SHOWMINIMIZED"));
	ComboBox_AddString(hctl, _T("3 SW_MAXIMIZED"));
	ComboBox_AddString(hctl, _T("4 SW_SHOWNOACTIVATE"));
	ComboBox_AddString(hctl, _T("5 SW_SHOW"));
	ComboBox_AddString(hctl, _T("6 SW_MINIMIZE"));
	ComboBox_AddString(hctl, _T("7 SW_SHOWMINNOACTIVE"));
	ComboBox_AddString(hctl, _T("8 SW_SHOWNA"));
	ComboBox_AddString(hctl, _T("9 SW_RESTORE"));
	ComboBox_AddString(hctl, _T("10 SW_SHOWDEFAULT"));
	ComboBox_AddString(hctl, _T("11 SW_FORCEMINIMIZE"));

	ComboBox_SetCurSel(hctl, 1);
}


BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	chSETDLGICONS(hdlg, IDI_WINMAIN);

	vaSetDlgItemText(hdlg, IDC_EDIT_INFO, 
		_T("Call ShowWindow() on specified HWND.\r\n")
		_T("HWND is initially filled with my HWND 0x%08X.\r\n"),
		(UINT)hdlg);

	vaSetDlgItemText(hdlg, IDC_EDIT_HWND, _T("%08X"), (UINT)hdlg);

	Fill_DropdownList(hdlg);

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_EDIT_HWND));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
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
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)&dlgdata);

	return 0;
}
