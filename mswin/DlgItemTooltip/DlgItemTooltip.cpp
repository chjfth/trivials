#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#include <mswin/CommCtrl.itc.h>
using namespace itc;

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

HFONT g_hfTT;
HWND g_hwndTT;
RECT g_rcText;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	TCHAR textbuf[200];

	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		++(prdata->clicks);
		_sntprintf_s(textbuf, _TRUNCATE, _T("Clicks: %d"), prdata->clicks);
		SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, textbuf);

		InvalidateRect(GetDlgItem(hdlg, IDC_LABEL1), NULL, TRUE);
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

void create_demo_tooltip(HWND hdlg)
{
	g_hwndTT = CreateWindowEx(WS_EX_TRANSPARENT, 
		TOOLTIPS_CLASS, 
		NULL, // window title
		TTS_NOPREFIX | TTS_NOANIMATE | TTS_ALWAYSTIP,
		0, 0, 0, 0,
		hdlg, NULL, nullptr, NULL);
	assert(g_hwndTT);
	if(!g_hwndTT)
		return;

	HWND hwndLabel = GetDlgItem(hdlg, IDC_LABEL1);
	TCHAR szLabel[100] = {};
	GetDlgItemText(hdlg, IDC_LABEL1, szLabel, ARRAYSIZE(szLabel));

	g_hfTT = GetWindowFont(hwndLabel);
	SetWindowFont(g_hwndTT, g_hfTT, FALSE);

	TOOLINFO ti = { sizeof(ti) };
	ti.uFlags =  TTF_SUBCLASS | TTF_IDISHWND;
	ti.hwnd = hdlg;
	ti.uId = (UINT_PTR)hwndLabel; // 0;
	ti.lpszText = szLabel;
	
	RECT rcLabel = {};
	GetWindowRect(hwndLabel, &rcLabel);
	g_rcText = rcLabel;

	MapWindowPoints(HWND_DESKTOP, hdlg, (POINT*)&g_rcText, 2);

	ScreenToClient(hdlg, (POINT*)&rcLabel.left);
	ScreenToClient(hdlg, (POINT*)&rcLabel.right);

	ti.rect = rcLabel;

	BOOL succ = SendMessage(g_hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);
	assert(succ);

	// Chj: Set tooltip delay-times.
	succ = SendMessage(g_hwndTT, TTM_SETDELAYTIME, TTDT_INITIAL, 10);
	succ = SendMessage(g_hwndTT, TTM_SETDELAYTIME, TTDT_AUTOPOP, 12000);
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	create_demo_tooltip(hdlg);
	
	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

// new >>>

LRESULT OnTooltipShow(HWND hdlg, NMHDR *pnm)
{
// 	TOOLINFO ti = {sizeof(ti)};
// 	ti.hwnd = hdlg;
// 	BOOL succ = SendMessage(pnm->hwndFrom, TTM_GETTOOLINFO, 0, (LPARAM)&ti);

	RECT rcLabel = {};
	HWND hwndLabel = GetDlgItem(hdlg, IDC_LABEL1);
	GetWindowRect(hwndLabel, &rcLabel);
	MapWindowPoints(HWND_DESKTOP, hdlg, (POINT*)&rcLabel, 2);
	RECT rc1 = rcLabel;

	MapWindowRect(hdlg, NULL, &rc1);
	RECT rc2 = rc1; 

	// Now, rc1 is the text-label's window Rect, and the text may have been
	// vertically centered, not necessarily rendered from y-top.
	// So we need to find out y-start of the text.
	HDC hdc = GetDC(hdlg);
	SelectFont(hdc, g_hfTT);
	TEXTMETRIC tm = {};
	GetTextMetrics(hdc, &tm);

	rc1.top += (rc1.bottom - rc1.top - tm.tmHeight)/2; // quite accurate

	// Another way to calculate y-top
	RECT rcText = rc2;
	TCHAR szText[256];
	GetDlgItemText(hdlg, IDC_LABEL1, szText, ARRAYSIZE(szText));
	DrawText(hdc, szText, -1, &rcText, DT_SINGLELINE|DT_CALCRECT);
	rc2.top += (rc2.bottom - rc2.top - (rcText.bottom-rcText.top))/2;

	assert( EqualRect(&rc1, &rc2) ); // hope so

	SendMessage(pnm->hwndFrom, TTM_ADJUSTRECT, TRUE, (LPARAM)&rc1);
	SetWindowPos(pnm->hwndFrom, 0, rc1.left, rc1.top, 0, 0,
		SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	return TRUE; // suppress default positioning
}

LRESULT Dlg_OnNotify(HWND hwnd, int idFrom, NMHDR *pnm)
{
	vaDbgTs(_T("Dlg_OnNotify() idFrom=%d , code=%s"),
		idFrom, ITCSv(pnm->code, TTN_xxx));

	//if (pnm->hwndFrom == g_hwndTT)  
	{
		switch (pnm->code) {
		case TTN_SHOW:
			return OnTooltipShow(hwnd, pnm);
		}
	}
	return 0;   
}

void Dlg_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	vaDbgTs(_T("WM_MOUSEMOVE: x=%d, y=%d"), x, y);
}


INT_PTR WINAPI UserDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);

		HANDLE_dlgMSG(hdlg, WM_NOTIFY,        Dlg_OnNotify);

		HANDLE_dlgMSG(hdlg, WM_MOUSEMOVE,     Dlg_OnMouseMove);
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
