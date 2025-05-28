#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <assert.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include <mswin/CommCtrl.itc.h>
#include <mswin/WinUser.itc.h>
using namespace itc;

#include "iversion.h"

#include "../utils.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;


// From: L:\viewsrc\nt5src\Source\Win2K3\NT\shell\comctl32\v6\tooltips.cpp
#define TTT_INITIAL        1
#define TTT_RESHOW         2
#define TTT_POP            3
#define TTT_AUTOPOP        4
#define TTT_FADESHOW       5
#define TTT_FADEHIDE       6

const Enum2Val_st _e2v_TTT_xxx[] =
{
	ITC_NAMEPAIR(TTT_INITIAL),//        1
	ITC_NAMEPAIR(TTT_RESHOW),//         2
	ITC_NAMEPAIR(TTT_POP),//            3
	ITC_NAMEPAIR(TTT_AUTOPOP),//        4
	ITC_NAMEPAIR(TTT_FADESHOW),//       5
	ITC_NAMEPAIR(TTT_FADEHIDE),//       6
};
CInterpretConst TTT_xxx(_e2v_TTT_xxx, ITCF_SINT);

struct DlgPrivate_st
{
	HWND hdlg;
	HWND hwndTT;
	int uicTtUse;
};


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDC_RADIO1:
	case IDC_RADIO2:
		prdata->uicTtUse = id;
		break;

	case IDOK:
	case IDCANCEL:
	{
		EndDialog(hdlg, id);
		break;
	}
	}}
}

LRESULT CALLBACK 
SubclassProc_MyTooltip(HWND hwndTT, UINT uMsg, WPARAM wParam, LPARAM lParam, 
	UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	assert(uIdSubclass==0);
	
	DlgPrivate_st &prdata = *(DlgPrivate_st*)(dwRefData);

	// Debug-peeking window messages to the tooltip-window.
	if(uMsg==TTM_WINDOWFROMPOINT)
	{
		POINT &pt = *(POINT*)lParam;

		// Chj: We will tamper the `pt`, cheat that "mouse position" is from 
		// current prdata.uicTtUse.

		HWND hwndTool = GetDlgItem(prdata.hdlg, prdata.uicTtUse);
		assert(IsWindow(hwndTool));
		
		RECT rc;
		GetWindowRect(hwndTool, &rc);
		POINT ptnew = {(rc.left+rc.right)/2 , (rc.top+rc.bottom)/2};

		vaDbgTs(_T("Tooltip sees TTM_WINDOWFROMPOINT, pt=[%d,%d] -> [%d,%d]"), 
			 pt.x, pt.y, ptnew.x, ptnew.y);

		pt = ptnew; // tamper this pt, 
	}
	else if(uMsg==WM_TIMER)
	{
		vaDbgTs(_T("Tooltip sees WM_TIMER, timerId=%s"), ITCSv((int)wParam, TTT_xxx));
	}
	else
	{
		vaDbgTs(_T("Tooltip sees wmsg: %s"), ITCSv(uMsg, TTM_xxx_WM_xxx));
	}

	LRESULT lre = DefSubclassProc(hwndTT, uMsg, wParam, lParam);


	if(uMsg==TTM_WINDOWFROMPOINT)
	{
		vaDbgTs(_T("TTM_WINDOWFROMPOINT returns HWND=0x%X"), (UINT)lre);
	}
	return lre;
}


BOOL CreateTooltip_3tools(HWND hdlg)
{
	DlgPrivate_st &prdata = *(DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	HWND &hwndTT = prdata.hwndTT;

	hwndTT = CreateWindowEx(WS_EX_TRANSPARENT, 
		TOOLTIPS_CLASS, 
		NULL, // window title
		TTS_NOPREFIX | TTS_NOANIMATE | TTS_ALWAYSTIP,
		0, 0, 0, 0,
		hdlg, NULL, g_hinstExe, NULL);
	if (!prdata.hwndTT)
		return FALSE;

	// Add tool 0 

	TOOLINFO ti = { sizeof(ti) };
	ti.uFlags = TTF_IDISHWND | TTF_TRANSPARENT | TTF_SUBCLASS;
	ti.hwnd = NULL;

	ti.uId = (UINT_PTR)GetDlgItem(hdlg, IDC_ICON1);
	ti.lpszText = (TCHAR*)_T("Icon's tooltip");
	LRESULT succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);
	assert(succ);
	vaDbgTs(_T("Add tool uic=%d, hwnd=0x%X"), IDC_ICON1, ti.uId);

	ti.uId = (UINT_PTR)GetDlgItem(hdlg, IDC_RADIO1);
	ti.lpszText = (TCHAR*)_T("Tooltip One");
	succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);
	assert(succ);
	vaDbgTs(_T("Add tool uic=%d, hwnd=0x%X"), IDC_RADIO1, ti.uId);

	ti.uId = (UINT_PTR)GetDlgItem(hdlg, IDC_RADIO2);
	ti.lpszText = (TCHAR*)_T("Tooltip Two");
	succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);
	assert(succ);
	vaDbgTs(_T("Add tool uic=%d, hwnd=0x%X"), IDC_RADIO2, ti.uId);

	// Make the tooltip appear immediately (0ms), instead of delaying 500ms.
	SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_INITIAL, 0);

	succ = SetWindowSubclass(hwndTT, SubclassProc_MyTooltip, 0, (DWORD_PTR)&prdata);
	return (BOOL)succ;
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st &prdata = *(DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)&prdata);

//	HWND hwndIcon = GetDlgItem(hdlg, IDC_ICON1);
//	HICON hIcon = LoadIcon(g_hinstExe, MAKEINTRESOURCE(IDI_WINMAIN));
//	Static_SetIcon(hwndIcon, hIcon);

	prdata.hdlg = hdlg;

	prdata.uicTtUse = IDC_RADIO1;
	CheckDlgButton(hdlg, prdata.uicTtUse, TRUE);

	CreateTooltip_3tools(hdlg);

	SetWindowPos(hdlg, 0, 0,0,0,0,
		SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

	return TRUE;
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

	DlgPrivate_st dlgdata = { };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, (LPARAM)&dlgdata);

	return 0;
}
