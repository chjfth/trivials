#include "shareinc.h"

#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#include <mswin/JULayout2.h>

#include <mswin/winuser.itc.h>

//#include "ModelessTtdemo.h"
#include "TtDlgForUic.h"
#include "TtDlgForRectArea.h"
#include "TtDlgMultiline.h"
#include "TtDlgTrackingToolTip.h"
#include "TtDlgInplaceSimplest.h"
#include "TtDlgInplaceToolTip.h"

#include "dlgbox-helper.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

BOOL g_isTTS_BALLOON = FALSE;
BOOL g_isTTF_CENTERTIP = FALSE;
BOOL g_isWS_EX_TRANSPARENT = TRUE;


struct DlgPrivate_st
{
	int clicks;
	CModelessChild *ptdForUic; // HWND hdlgTtForUic;
	CModelessChild *ptdForRectArea; // HWND hdlgTtForRectArea;
	CModelessChild *ptdMultiline;
	CModelessChild *ptdTrackingTooltip1;
	CModelessChild *ptdTrackingTooltipMisc;
	CModelessChild *ptdInplaceSimplest;
	CModelessChild *ptdInplaceTooltip;
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

	case IDB_MultilineTooltip:
		CModelessTtDemo::LaunchTootipDemoChildDlg<CTtDlgMultiline>(
			_T("IDD_TooltipMultiline"), IDD_TooltipMultiline, hdlgMain, &ctx.ptdMultiline);
		break;

	case IDB_TrackingTooltipConcise:
		CModelessTtDemo::LaunchTootipDemoChildDlg<CTtDlgTrackingTooltip_concise>(
			_T("IDD_TrackingTooltip1"), IDD_TrackingConcise, hdlgMain, &ctx.ptdTrackingTooltip1);
		break;

	case IDB_TrackingTooltipMisc:
		CModelessTtDemo::LaunchTootipDemoChildDlg<CTtDlgTrackingTooltip_misc>(
			_T("IDD_TrackingTooltipMisc"), IDD_TrackingMisc, hdlgMain, &ctx.ptdTrackingTooltipMisc);
		break;

	case IDB_InplaceSimplest:
		CModelessTtDemo::LaunchTootipDemoChildDlg<CTtDlgInplaceSimplest>(
			_T("IDD_InplaceSimplest"), IDD_InplaceSimplest, hdlgMain, &ctx.ptdInplaceSimplest);
		break;

	case IDB_InplaceTooltip:
	{
		BOOL bTrans = 0;
		int pt = GetDlgItemInt(hdlgMain, IDE_FontsizePt, &bTrans, bSigned_FALSE);
		if(bTrans && pt>0)
		{
			void *dlgtmpl = LoadAndPatchDialogTemplate(g_hinstExe, 
				MAKEINTRESOURCE(IDD_InplaceComplex), pt);
			assert(dlgtmpl);
		}

		CModelessTtDemo::LaunchTootipDemoChildDlg<CTtDlgInplaceTooltip>(
			_T("IDD_InplaceTooltip"), IDD_InplaceComplex, hdlgMain, &ctx.ptdInplaceTooltip);
		break;
	}
	case IDCK_TTS_BALLOON:
	{
		g_isTTS_BALLOON = IsDlgButtonChecked(hdlg, IDCK_TTS_BALLOON);
		break;
	}
	case IDCK_TTF_CENTERTIP:
	{
		g_isTTF_CENTERTIP = IsDlgButtonChecked(hdlg, IDCK_TTF_CENTERTIP);
		break;
	}
	case IDCK_WsexTransparent:
	{
		g_isWS_EX_TRANSPARENT = IsDlgButtonChecked(hdlg, IDCK_WsexTransparent);
		break;
	}
	case IDOK:
	case IDCANCEL:
	{
		PostQuitMessage(0); // EndDialog(hdlg, id);
		break;
	}
	}}
}

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControls(0, 0, 0, 0,
		IDCK_EnableMultiline, IDE_LineWidth, IDS_LineWidth, 
		IDB_MultilineTooltip,
		-1);
	jul->AnchorControls(0, 0, 100, 100, // right-side
		IDGB_MultilineTooltip, IDE_MultilineText,
		-1);

	jul->AnchorControls(0, 100, 0, 100, // left-bottom corner
		IDB_TrackingTooltipConcise, IDS_TttOffsetX, IDE_TttOffsetX, IDS_TttOffsetY, IDE_TttOffsetY
		,
		IDGB_TrackingTooltip, IDB_TrackingTooltipMisc, IDCK_TTF_TRANSPARENT,
		IDCK_TTF_TRACK, IDE_DelayAfterTooltipText, IDS_DelayAfterTooltipText,
		IDCK_ClientToScreen, IDCK_TTF_ABSOLUTE
		,
		IDB_InplaceSimplest, IDB_InplaceComplex, IDS_FontsizePt, IDE_FontsizePt,
		-1);


	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	vaDbgTs(_T("hdlgMain = 0x%08X"), hdlg);

	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st &ctx = *(DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)&ctx);
	
	vaSetWindowText(hdlg, _T("msdnTooltipDemo v%d.%d.%d"), 
		msdnTooltip_VMAJOR, msdnTooltip_VMINOR, msdnTooltip_VPATCH);

	CheckDlgButton(hdlg, IDCK_TTS_BALLOON, g_isTTS_BALLOON);
	CheckDlgButton(hdlg, IDCK_TTF_CENTERTIP, g_isTTF_CENTERTIP);
	CheckDlgButton(hdlg, IDCK_WsexTransparent, g_isWS_EX_TRANSPARENT);

	CheckDlgButton(hdlg, IDCK_EnableMultiline, TRUE);
	SetDlgItemText(hdlg, IDE_LineWidth, _T("200"));
	SetDlgItemText(hdlg, IDE_MultilineText, _T("Sample Line 1\r\nSample Line 2"));

	SetDlgItemInt(hdlg, IDE_TttOffsetX,  10, bSigned_TRUE);
	SetDlgItemInt(hdlg, IDE_TttOffsetY, -20, bSigned_TRUE);

	CheckDlgButton(hdlg, IDCK_TTF_TRANSPARENT, TRUE);
	CheckDlgButton(hdlg, IDCK_TTF_TRACK, TRUE);
	CheckDlgButton(hdlg, IDCK_TTF_ABSOLUTE, TRUE);
	CheckDlgButton(hdlg, IDCK_ClientToScreen, TRUE);
	SetDlgItemText(hdlg, IDE_DelayAfterTooltipText, _T("0"));

	SetDlgItemText(hdlg, IDE_FontsizePt, _T("8"));

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDB_TooltipForUic));
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

		// If some demo dlg needs Tab-key switching focus, we should add these:

		if(ctx.ptdForUic && IsDialogMessage(ctx.ptdForUic->GetHdlg(), &msg))
			continue;

		if(ctx.ptdMultiline && IsDialogMessage(ctx.ptdMultiline->GetHdlg(), &msg))
			continue;

		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	return 0;
}
