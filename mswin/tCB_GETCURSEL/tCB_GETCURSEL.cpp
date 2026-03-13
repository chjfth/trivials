#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#define CHHI_ALL_IMPL

#include <vaDbgTs.h>
#include <mswin/utils_wingui.h>
#include <mswin/JULayout2.h>
#include <mswin/WinUser.itc.h>

using namespace itc;

// #pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
// -- [2026-02-23] Chj note: Enabling VisualStyle or not, makes different behavior when [ComboBox_ResetContent each time] is ticked.

HINSTANCE g_hinstExe;

int g_count = 0;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

void myResetCombobox(HWND hdlg, int codeNotify)
{
	HWND hcb = GetDlgItem(hdlg, IDC_COMBO1);
	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	int selwas = ComboBox_GetCurSel(hcb);

	vaAppendText_mled(hedit, 
		_T("[#%d]     Before ComboBox_ResetContent(), codeNotify=%d, selwas=%d\r\n"),
		g_count,
		codeNotify, selwas
		);

	ComboBox_ResetContent(hcb);

	ComboBox_AddString(hcb, _T("Item0"));
	ComboBox_AddString(hcb, _T("Item1"));
	ComboBox_AddString(hcb, _T("Item2"));
	ComboBox_AddString(hcb, _T("Item3"));

	ComboBox_SetCurSel(hcb, 2);
}

void myPeekCombobox(HWND hdlg, int codeNotify)
{
	TCHAR timestr[40];

	HWND hcbx = GetDlgItem(hdlg, IDC_COMBO1);
	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	int cursel = ComboBox_GetCurSel(hcbx);

	vaAppendText_mled(hedit, _T("[#%d]%s codeNotify=%s , cursel=%d\r\n"),
		g_count, va_now_hms(timestr, ARRAYSIZE(timestr)),
		ITCSvn(codeNotify, CBN_xxx_ComboBox), cursel
		);
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st &prdata = *(DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		break;
	}
	case IDC_COMBO1:
	{
		g_count++;

		int isChk = Button_GetCheck(GetDlgItem(hdlg, IDC_CHECK1));
		
		if(isChk) {
			myResetCombobox(hdlg, codeNotify);
		}

		myPeekCombobox(hdlg, codeNotify);

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

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
	jul->AnchorControl(50,100, 50,100, IDC_BUTTON1);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st &prdata = *(DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)&prdata);
	
	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		tCB_GETCURSEL_VMAJOR, tCB_GETCURSEL_VMINOR, tCB_GETCURSEL_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);
	
	const TCHAR *helptext = 
		_T("Click to dropdown the combobox, then click elsewhere to dismiss the dropdown.\r\n")
		_T("Observe the CBN_xxx notification messages meanwhile.\r\n\r\n")
		;

	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, helptext);

	Dlg_EnableJULayout(hdlg);

	myResetCombobox(hdlg, -1);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR WINAPI UserDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
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
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, (LPARAM)&dlgdata);

	return 0;
}
