#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

/*
Code behavior:
(1) IDCANCEL: Do NOT allow ESC close dialog.
(2) Move EndDialog() to WM_CLOSE.
(3) Clicking the Post-button, I will Post WM_KEYDOWN(ESC) to the multiline-editbox.

UI behavior: 
[1] When keyboard focus is on the button, ESC will not close the dlgbox(expected behavior).
[2] When keyboard focus is on the editbox, ESC will close the dlgbox, why?
[3] To explain [2], click the Post-button, we see that the dlgbox get clocsed as well.
*/

#include "iversion.h"

#include "..\utils.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	
	switch (id) 
	{
	case IDOK:
		EndDialog(hdlg, id);
		break;
	case IDCANCEL:
		// Do NOT allow ESC close dialog.
		// EndDialog(hdlg, id);
		OutputDebugString(_T("Not calling EndDialog() on IDCANCEL.\n"));
		break;

	case IDC_BUTTON1:
		{
			HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
			PostMessage(hedit, WM_KEYDOWN, VK_ESCAPE, 0x10001);
			break;
		}
	}
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		DlgEscNoClose1_VMAJOR, DlgEscNoClose1_VMINOR, DlgEscNoClose1_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);
	
	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
		_T("Pressing ESC on this editbox will close the dialog box. Why?"));

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

void Dlg_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 4);
}

INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);

		HANDLE_dlgMSG(hdlg, WM_CLOSE, Dlg_OnClose);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	DlgPrivate_st dlgdata = { _T("Hello.\r\nPrivate string here.") };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)&dlgdata);

	return 0;
}
