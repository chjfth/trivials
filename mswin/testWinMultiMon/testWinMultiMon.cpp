#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "iversion.h"

#include "../utils.h"

#include <CHHI_vaDBG_is_vaDbgTs.h>

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#define JAutoBuf_IMPL
#include <JAutoBuf.h>

#define WinMultiMon_IMPL
#define WinMultiMon_DEBUG
#include <mswin/WinMultiMon.h>


void ui_refresh(HWND hdlg)
{
	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
	WinErr_t winerr = 0;

	JAutoBuf<OneMonitorInfo_st> abMonInfo;
	do
	{
		winerr = doEnumDisplayMonitors(abMonInfo, abMonInfo, abMonInfo);
	} while (winerr == ERROR_MORE_DATA);

	if (winerr)
	{
		vaSetWindowText(hedit, _T("Something wrong in doEnumDisplayMonitors(), winerr=%d."), winerr);
		return;
	}

	int nMonitors = (int)abMonInfo.Eles();
	vaSetWindowText(hedit, _T("Total %d monitors:\r\n"), nMonitors);

	for (int i = 0; i < nMonitors; i++)
	{
		OneMonitorInfo_st &mi = abMonInfo[i];

		TCHAR rctext[40] = {};
		vaAppendText_mled(hedit,
			_T("[#%d] \"%s\" %s\r\n"),
			i + 1,
			mi.szDevice,
			RECTtext(mi.rcMonitor, rctext)
		);
	}
}


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		ui_refresh(hdlg);
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

	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		testWinMultiMon_VMAJOR, testWinMultiMon_VMINOR, testWinMultiMon_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);
	
	Dlg_EnableJULayout(hdlg);

	ui_refresh(hdlg);

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
	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, NULL);

	return 0;
}
