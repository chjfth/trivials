#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include "resource.h"

#include "iversion.h"

#include "../utils.h"


#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

void ShowDlgBaseUnits(HWND hdlg)
{
	ULONG bus = GetDialogBaseUnits();

	int dlgx = 4, dlgy = 8;
	RECT rc = {0,0, dlgx,dlgy};
	BOOL succ = MapDialogRect(hdlg, &rc);
	assert(succ);

	vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
		_T("GetDialogBaseUnits() = [%d,%d]\r\n")
		_T("MapDialogRect(x=%d, y=%d) = [%d,%d]\r\n")
		,
		LOWORD(bus), HIWORD(bus),
		dlgx, dlgy, rc.right, rc.bottom
		);
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		ShowDlgBaseUnits(hdlg);
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

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	chSETDLGICONS(hdlg, IDI_WINMAIN);

	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		DlgBaseUnits_VMAJOR, DlgBaseUnits_VMINOR, DlgBaseUnits_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);
	
	ShowDlgBaseUnits(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
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

	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, NULL);

	return 0;
}
