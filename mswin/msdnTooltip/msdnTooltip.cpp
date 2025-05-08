#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

#define ASSERT_DLG_SUCC(hdlg, dlg_rcid) \
	if((hdlg)==NULL) { \
		vaMsgBox(NULL, MB_OK|MB_ICONERROR, NULL, _T("Main dialog creation fail: ") _T(#dlg_rcid)); \
		exit(4); \
	}


struct DlgPrivate_st
{
	int clicks;
	HWND hdlgTtForUic;
	HWND hdlgTtForRectArea;
};


INT_PTR WINAPI DlgProc_TtForUic(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	if(uMsg==WM_INITDIALOG)
	{
		vaDbgTs(_T("DlgProc_TtForUic init..."));
	}
	return 0;
/*
	switch (uMsg) 
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		if( GET_WM_COMMAND_ID(wParam, lParam)==IDCANCEL )
			EndDialog(hdlg, 0);
		break;
	}
	return FALSE;
*/
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	HWND hdlgMain = hdlg;
	DlgPrivate_st &ctx = *(DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDB_TooltipForUic:
	{
		HWND &hsub = ctx.hdlgTtForUic;
		if(!hsub)
		{
			hsub = CreateDialog(g_hinstExe, MAKEINTRESOURCE(IDD_TooltipForUic), 
				hdlgMain, DlgProc_TtForUic);
			ASSERT_DLG_SUCC(hsub, IDD_TooltipForUic);
		}
		else
		{
			SetForegroundWindow(hsub);
		}
		break;
	}
	case IDB_TooltipForRectArea:
		break;
	case IDOK:
	case IDCANCEL:
	{
		PostQuitMessage(0); // EndDialog(hdlg, id);
		break;
	}
	}}
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st &ctx = *(DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)&ctx);
	
	vaSetWindowText(hdlg, _T("msdnTooltipDemo v%d.%d.%d"), 
		msdnTooltip_VMAJOR, msdnTooltip_VMINOR, msdnTooltip_VPATCH);
	
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


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int nCmdShow) 
{
	g_hinstExe = hinstExe;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	DlgPrivate_st ctx = { };
	
	HWND hdlgMain = CreateDialogParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), 
		NULL, // no parent window
		UserDlgProc, (LPARAM)&ctx);
	ASSERT_DLG_SUCC(hdlgMain, IDD_WINMAIN);

	// ShowWindow(hdlgMain, nCmdShow); // not necc bcz of WS_VISIBLE in .rc

	MSG msg = {};

	while (GetMessage (&msg, NULL, 0, 0))
	{
		if(IsDialogMessage(hdlgMain, &msg))
			continue;

		if(IsDialogMessage(ctx.hdlgTtForUic, &msg))
			continue;

		if(IsDialogMessage(ctx.hdlgTtForRectArea, &msg))
			continue;

		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}



	return 0;
}
