/*
	This program is roughly the same as DlgItemTooltip.
	What's new is that user can change TTS_BALLOON and TTF_CENTERTIP after toolip 
	has been created.
*/

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

#include <mswin/WinUser.itc.h>
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

HWND create_demo_tooltip(HWND hdlg, BOOL isWsExTransparent, BOOL isTtfTransparent,
	BOOL isTTS_BALLOON, BOOL isTTF_CENTERTIP)
{
	// Will return tooltip HWND.

	HWND hwndTT = CreateWindowEx(
		isWsExTransparent ? WS_EX_TRANSPARENT : 0, 
		TOOLTIPS_CLASS, 
		NULL, // window title
		TTS_NOPREFIX | TTS_NOANIMATE | TTS_ALWAYSTIP | (isTTS_BALLOON?TTS_BALLOON:0),
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
	ti.uFlags |= (isTTF_CENTERTIP ? TTF_CENTERTIP : 0);
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
	succ = SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_AUTOPOP, 29000); 
	// -- show tooltip for 29 seconds; too-large(eg 59000) will revert to 5000 default.

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
	vaDbgTs(_T("Dlg - WM_MOUSEMOVE: x=%d, y=%d"), x, y);
}

LRESULT CALLBACK 
SubclassProc_MonitorTooltip(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, 
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	assert(uIdSubclass==0);
	HWND hdlg = (HWND)(dwRefData);

	vaDbgTs(_T("Tooltip sees wmsg: %s"), ITCSv(uMsg, TTM_xxx_WM_xxx));

	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

void do_IDB_CreateTooltip(HWND hdlg)
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	HWND &hwndTT = prdata->hwndTooltip;
	BOOL succ = 0;

	if(hwndTT==NULL)
	{
		BOOL isWsExTransp = IsDlgButtonChecked(hdlg, IDCK_WsExTransparent);
		BOOL isTtfTransp = IsDlgButtonChecked(hdlg, IDCK_TtfTransparent);
		
		BOOL isTTS_BALLOON = IsDlgButtonChecked(hdlg, IDCK_TTS_BALLOON);
		BOOL isTTF_CENTERTIP = IsDlgButtonChecked(hdlg, IDCK_TTF_CENTERTIP);

		hwndTT = create_demo_tooltip(hdlg, isWsExTransp, isTtfTransp,
			isTTS_BALLOON, isTTF_CENTERTIP);
		if(!hwndTT)
		{
			SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, _T("Unexpect! Tooltip window creation fail."));
			return;
		}

		SetDlgItemText(hdlg, IDB_CreateTooltip, _T("&Destroy tooltip"));
		SetDlgItemText(hdlg, IDC_LABEL1, s_TooltipPrompt);

		DisableDlgItem(hdlg, IDCK_WsExTransparent);
		DisableDlgItem(hdlg, IDCK_TtfTransparent);

		succ = SetWindowSubclass(hwndTT, SubclassProc_MonitorTooltip, 0, (DWORD_PTR)hdlg);
		assert(succ);
	}
	else
	{
		BOOL succ = 0;
		succ = RemoveWindowSubclass(hwndTT, SubclassProc_MonitorTooltip, 0);
		assert(succ);

		succ = DestroyWindow(hwndTT);
		assert(succ);

		hwndTT = NULL;
		vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
			_T("Destroy tooltip-window [%s]"), succ?_T("Success"):_T("Fail"));

		SetDlgItemText(hdlg, IDB_CreateTooltip, _T("&Create tooltip"));
		SetDlgItemText(hdlg, IDC_LABEL1, s_InitPrompt);

		EnableDlgItem(hdlg, IDCK_WsExTransparent);
		EnableDlgItem(hdlg, IDCK_TtfTransparent);
	}
}

void change_WinStyleBits(HWND hwnd, DWORD style_bits, BOOL on_or_off)
{
	DWORD now_style = GetWindowStyle(hwnd);
	if(on_or_off)
		now_style |= style_bits;
	else
		now_style &= ~style_bits;

	SetWindowLong(hwnd, GWL_STYLE, now_style);
}


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDB_CreateTooltip:
	{
		do_IDB_CreateTooltip(hdlg);
		break;
	}
	case IDCK_TTS_BALLOON:
	{
		if(!prdata->hwndTooltip)
			break;

		BOOL on_or_off = IsDlgButtonChecked(hdlg, IDCK_TTS_BALLOON);
		change_WinStyleBits(prdata->hwndTooltip, TTS_BALLOON, on_or_off);
		break;
	}
	case IDCK_TTF_CENTERTIP:
	{
		if(!prdata->hwndTooltip)
			break;

		BOOL on_or_off = IsDlgButtonChecked(hdlg, IDCK_TTF_CENTERTIP);

		TOOLINFO ti = {sizeof(ti)};
		ti.hwnd = hdlg;
		ti.uId = (UINT_PTR)GetDlgItem(hdlg, IDC_LABEL1);
		BOOL succ = SendMessage(prdata->hwndTooltip, TTM_GETTOOLINFO, 0, (LPARAM)&ti);
		assert(succ);
		
		// Note: ti.lpszText is NULL here, we must re-assign our text.
		// otherwise, tooltip text will be empty and we will not see tooltip any more.
		ti.lpszText = const_cast<TCHAR*>(s_TooltipPrompt);

		if(on_or_off)
			ti.uFlags |= TTF_CENTERTIP;
		else
			ti.uFlags &= ~TTF_CENTERTIP;

		SendMessage(prdata->hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&ti);
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

	vaSetWindowText(hdlg, _T("DlgItemTooltip2 v%d.%d.%d"), 
		DlgItemTooltip2_VMAJOR, DlgItemTooltip2_VMINOR, DlgItemTooltip2_VPATCH);

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
