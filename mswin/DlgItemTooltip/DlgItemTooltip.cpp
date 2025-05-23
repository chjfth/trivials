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

#include <mswin/win32clarify.h>

#include <mswin/CommCtrl.itc.h>
using namespace itc;

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;


struct DlgPrivate_st
{
	HWND hwndTooltip;
};

const TCHAR *s_InitPrompt = _T("Click [Create tooltip] button to go on.");
const TCHAR *s_TooltipPrompt = _T("Hover mouse over me to show in-place tooltip.");

HWND create_demo_tooltip(HWND hdlg, BOOL isWsExTransparent, BOOL isTtfTransparent)
{
	// Will return tooltip HWND.

	HWND hwndTT = CreateWindowEx(
		isWsExTransparent ? WS_EX_TRANSPARENT : 0, 
		TOOLTIPS_CLASS, 
		NULL, // window title
		TTS_NOPREFIX | TTS_NOANIMATE | TTS_ALWAYSTIP,
		0, 0, 0, 0,
		hdlg, NULL, 
		nullptr, // Chj: deliberate NULL hInstance, looks OK
		NULL);
	assert(hwndTT);
	if(!hwndTT)
		return NULL;

	vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
		_T("Tooltip-HWND = 0x%08X"), hwndTT);

	HWND hwndLabel = GetDlgItem(hdlg, IDC_LABEL1);

	HFONT hfont = GetWindowFont(hwndLabel);
	SetWindowFont(hwndTT, hfont, FALSE);

	TOOLINFO ti = { sizeof(ti) };
	ti.uFlags =  TTF_SUBCLASS | TTF_IDISHWND | (isTtfTransparent ? TTF_TRANSPARENT : 0);
	ti.hwnd = hdlg;
	ti.uId = (UINT_PTR)hwndLabel;
	ti.lpszText = const_cast<TCHAR*>(s_TooltipPrompt);
	
	RECT rcLabel = {};
	GetWindowRect(hwndLabel, &rcLabel);

	MapWindowPoints(HWND_DESKTOP, hdlg, (POINT*)&rcLabel, 2);


	LRESULT succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);
	assert(succ);

	// Chj: Set tooltip delay-times.
	succ = SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_INITIAL, 10);
	succ = SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_AUTOPOP, 12000);

	return hwndTT;
}


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

	// Now, rc1 is the text-label's window Rect, and the label text may have been
	// vertically centered, not necessarily rendered from y-top.
	// So, in order for the tooltip to show-up exactly overlaying the label text,
	// we need to find out y-start of the label text.
	HDC hdc = GetDC(hwndLabel);
	HFONT hfont = GetWindowFont(hwndLabel);
	SelectFont(hdc, hfont);
	TEXTMETRIC tm = {};
	GetTextMetrics(hdc, &tm);

	rc1.top += (rc1.bottom - rc1.top - tm.tmHeight)/2; // quite accurate

	// Another way to calculate y-top
	RECT rcText = rc2;
	TCHAR szText[256];
	GetDlgItemText(hdlg, IDC_LABEL1, szText, ARRAYSIZE(szText));
	DrawText(hdc, szText, -1, &rcText, DT_SINGLELINE|DT_CALCRECT);
	rc2.top += (rc2.bottom - rc2.top - (rcText.bottom-rcText.top))/2;

	ReleaseDC(hwndLabel, hdc);

	assert( EqualRect(&rc1, &rc2) ); // hope so

	SendMessage(pnm->hwndFrom, TTM_ADJUSTRECT, TRUE, (LPARAM)&rc1);
	SetWindowPos(pnm->hwndFrom, 0, rc1.left, rc1.top, 0, 0,
		SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	return TRUE; // suppress default positioning
}


LRESULT Dlg_OnNotify(HWND hdlg, int idFrom, NMHDR *pnm)
{
	vaDbgTs(_T("Dlg_OnNotify() idFrom=%d(0x%X) , code=%s"),
		idFrom, idFrom, ITCSv(pnm->code, TTN_xxx));

	// memo: pnm->idFrom is the COMPLETE pointer-size version of idFrom.

	HWND hwndLabel = GetDlgItem(hdlg, IDC_LABEL1);

	if (pnm->idFrom == (UINT_PTR)hwndLabel)  
	{
		switch (pnm->code) {
		case TTN_SHOW:
			return OnTooltipShow(hdlg, pnm);
		}
	}
	return 0;   
}

void Dlg_OnMouseMove(HWND hdlg, int x, int y, UINT keyFlags)
{
	vaDbgTs(_T("WM_MOUSEMOVE: x=%d, y=%d"), x, y);
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	HWND &hwndTT = prdata->hwndTooltip;

	switch (id) 
	{{
	case IDB_CreateTooltip:
	{
		if(hwndTT==NULL)
		{
			BOOL isWsExTransp = IsDlgButtonChecked(hdlg, IDCK_WsExTransparent);
			BOOL isTtfTransp = IsDlgButtonChecked(hdlg, IDCK_TtfTransparent);
			hwndTT = create_demo_tooltip(hdlg, isWsExTransp, isTtfTransp);
			if(!hwndTT)
			{
				SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, _T("Unexpect! Tooltip window creation fail."));
				break;
			}

			SetDlgItemText(hdlg, IDB_CreateTooltip, _T("&Destroy tooltip"));
			SetDlgItemText(hdlg, IDC_LABEL1, s_TooltipPrompt);

			DisableDlgItem(hdlg, IDCK_WsExTransparent);
			DisableDlgItem(hdlg, IDCK_TtfTransparent);
		}
		else
		{
			BOOL succ = DestroyWindow(hwndTT);
			hwndTT = NULL;
			vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
				_T("Destroy tooltip-window [%s]"), succ?_T("Success"):_T("Fail"));

			SetDlgItemText(hdlg, IDB_CreateTooltip, _T("&Create tooltip"));
			SetDlgItemText(hdlg, IDC_LABEL1, s_InitPrompt);

			EnableDlgItem(hdlg, IDCK_WsExTransparent);
			EnableDlgItem(hdlg, IDCK_TtfTransparent);
		}
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
	jul->AnchorControl(0,100, 0,100, IDCK_WsExTransparent);
	jul->AnchorControl(0,100, 0,100, IDCK_TtfTransparent);
	jul->AnchorControl(100,100, 100,100, IDB_CreateTooltip);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);

	vaSetWindowText(hdlg, _T("DlgItemTooltip v%d.%d.%d"), 
		DlgItemTooltip_VMAJOR, DlgItemTooltip_VMINOR, DlgItemTooltip_VPATCH);

	SetDlgItemText(hdlg, IDC_LABEL1, s_InitPrompt);

	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG,
		_T("Note: If you tick neither WS_EX_TRANSPARENT or TTF_TRANSPARENT, the in-place tooltip will flicker."));

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDB_CreateTooltip));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
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

	DlgPrivate_st dlgdata = { };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, (LPARAM)&dlgdata);

	return 0;
}
