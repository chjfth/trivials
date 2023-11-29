#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"
#include "iversion.h"

#include "utils.h"

#define JULAYOUT_IMPL
#include "JULayout2.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
//	const TCHAR *mystr; 
	int count;
	RECT rwin; // window position last seen
};

#define MY_TIMER_ID ((void*)0x11) // only test, no use

void PrintMyPosition(HWND hdlg)
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	RECT rwin = {};
	GetWindowRect(hdlg,	&rwin);

	vaSetDlgItemText(hdlg, IDC_EDIT1, 
		_T("Now position: X=%d, Y=%d, size=(%d, %d)\r\n"), 
		rwin.left, rwin.top, (rwin.right-rwin.left), (rwin.bottom-rwin.top) );

	prdata->rwin = rwin;
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
//	TCHAR textbuf[200];

	switch (id) 
	{{
	case IDC_BUTTON1:
	{
/*
		++(prdata->clicks);
		_sntprintf_s(textbuf, _TRUNCATE, _T("Clicks: %d"), prdata->clicks);
		SetDlgItemText(hdlg, IDC_EDIT1, textbuf);
		InvalidateRect(GetDlgItem(hdlg, IDC_LABEL1), NULL, TRUE);
*/
		prdata->count = 0;
		SetDlgItemText(hdlg, IDC_EDIT1, _T(""));
		PrintMyPosition(hdlg);
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

void Dlg_OnMove(HWND hdlg, int x, int y)
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	prdata->count++;

	RECT &rwin = prdata->rwin;
	GetWindowRect(hdlg,	&rwin);

	vaSetDlgItemText(hdlg, IDC_EDIT1, 
		_T("[#%d] Move: X=%d, Y=%d\r\n"), 
		prdata->count, rwin.left, rwin.top);
}

void Dlg_OnSize(HWND hdlg, UINT state, int cx, int cy)
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	prdata->count++;

	RECT &rwin = prdata->rwin;
	GetWindowRect(hdlg,	&rwin);

	vaSetDlgItemText(hdlg, IDC_EDIT1, 
		_T("[#%d] Size: (%d, %d)\r\n"), 
		prdata->count, rwin.right-rwin.left, rwin.bottom-rwin.top);
}

void Dlg_OnTimer(HWND hdlg, UINT id)
{
	// M$ buggy, id should be of type UINT_PTR.
	assert((void*)id==MY_TIMER_ID);

	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	RECT rwin = {};
	GetWindowRect(hdlg,	&rwin);

	if(!EqualRect(&rwin, &prdata->rwin))
	{
		HWND hedit = GetDlgItem(hdlg, IDC_EDIT1);
		vaSetDlgItemText(hdlg, IDC_EDIT1, 
			_T("PANIC: Window-position changed to: X=%d, Y=%d, size=(%d, %d)\r\n"), 
			rwin.left, rwin.top, rwin.right-rwin.left, rwin.bottom-rwin.top);

		prdata->rwin = rwin;
	}
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
	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);

	SetDlgItemText(hdlg, IDC_LABEL1, _T("WM_MOVE and WM_SIZE messages will be notified here."));

	chSETDLGICONS(hdlg, IDI_WINMAIN);

	prdata->count = 0;
	PrintMyPosition(hdlg);

	SetTimer(hdlg, (UINT_PTR)MY_TIMER_ID, 100, NULL);

/*
	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		MonitorMyWinMove_VMAJOR, MonitorMyWinMove_VMINOR, MonitorMyWinMove_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);
	
	SetDlgItemText(hdlg, IDC_EDIT1, prdata->mystr);
*/

	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT1);
	jul->AnchorControl(50,100, 50,100, IDC_BUTTON1);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	
	return 0; // Let Dlg-manager respect SetFocus().
}

INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_MSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_MSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
		
		HANDLE_MSG(hdlg, WM_MOVE,          Dlg_OnMove);
		HANDLE_MSG(hdlg, WM_SIZE,          Dlg_OnSize);
		HANDLE_MSG(hdlg, WM_TIMER,         Dlg_OnTimer);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	DlgPrivate_st dlgdata = { };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)&dlgdata);

	return 0;
}
