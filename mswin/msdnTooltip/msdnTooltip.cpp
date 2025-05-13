#include "shareinc.h"

#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#include <mswin/JULayout2.h>

#include <mswin/winuser.itc.h>

//#include "ModelessTtdemo.h"
#include "TtDlgForUic.h"
#include "TtDlgForRectArea.h"


#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;


struct DlgPrivate_st
{
	int clicks;
	CModelessChild *ptdForUic; // HWND hdlgTtForUic;
	CModelessChild *ptdForRectArea; // HWND hdlgTtForRectArea;
};



void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	HWND hdlgMain = hdlg;
	DlgPrivate_st &ctx = *(DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDB_TooltipForUic:
		CModelessTtDemo::LaunchTootipDemoChildDlg<CTtDlgForUic>(
			_T("IDD_TooltipForUic"), IDD_TooltipForUic, hdlgMain, &ctx.ptdForUic);
		break;
	case IDB_TooltipForRectArea:
		CModelessTtDemo::LaunchTootipDemoChildDlg<CTtDlgForRectArea>(
			_T("IDD_TooltipForRectArea"), IDD_TooltipForRectArea, hdlgMain, &ctx.ptdForRectArea);
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
	vaDbgTs(_T("hdlgMain = 0x%08X"), hdlg);

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
	DlgPrivate_st &ctx = *(DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (uMsg) 
	{{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
	}}
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

	// ShowWindow(hdlgMain, nCmdShow); // not necessary bcz of WS_VISIBLE in .rc

	MSG msg = {};

	while (GetMessage (&msg, NULL, 0, 0))
	{
		if(IsDialogMessage(hdlgMain, &msg))
			continue;

		if(ctx.ptdForUic && IsDialogMessage(ctx.ptdForUic->GetHdlg(), &msg))
			continue;

		if(ctx.ptdForRectArea && IsDialogMessage(ctx.ptdForRectArea->GetHdlg(), &msg))
			continue;

		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}



	return 0;
}
