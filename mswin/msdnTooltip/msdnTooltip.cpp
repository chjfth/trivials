#include "shareinc.h"
#include <crtdbg.h>

#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#include <mswin/JULayout2.h>

#include <mswin/Combobox_EnableWideDrop.h>

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
int g_TTI_value = -1; // TTI_INFO, TTI_WARNING etc

struct DlgPrivate_st
{
	int clicks_null;

	CTtDlgForUic TtDlgForUic;
	CTtDlgForRectArea TtDlgForRectArea;
	CTtDlgMultiline TtDlgMultiline;
	CTtDlgTrackingTooltip_concise TtDlgTrackingTooltip_concise;
	CTtDlgTrackingTooltip_misc TtDlgTrackingTooltip_misc;
	CTtDlgInplaceSimplest TtDlgInplaceSimplest;
	CTtDlgInplaceComplex TtDlgInplaceComplex;

	CModelessChild *ptdForUic = nullptr;
	CModelessChild *ptdForRectArea = nullptr;
	CModelessChild *ptdMultiline = nullptr;
	CModelessChild *ptdTrackingTooltipConcise = nullptr;
	CModelessChild *ptdTrackingTooltipMisc = nullptr;
	CModelessChild *ptdInplaceSimplest = nullptr;
	CModelessChild *ptdInplaceComplex = nullptr;

	DlgPrivate_st() :
		TtDlgForUic(&ptdForUic),
		TtDlgForRectArea(&ptdForRectArea),
		TtDlgMultiline(&ptdMultiline),
		TtDlgTrackingTooltip_concise(&ptdTrackingTooltipConcise),
		TtDlgTrackingTooltip_misc(&ptdTrackingTooltipMisc),
		TtDlgInplaceSimplest(&ptdInplaceSimplest),
		TtDlgInplaceComplex(&ptdInplaceComplex)
	{}
};



void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	HWND hdlgMain = hdlg;
	DlgPrivate_st &ctx = *(DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDB_TooltipForUic:
		CModelessTtDemo::LaunchTootipDemoChildDlg(ctx.TtDlgForUic,
			_T("IDD_TooltipForUic"), IDD_TooltipForUic, hdlgMain, &ctx.ptdForUic);
		break;
	case IDB_TooltipForRectArea:
		CModelessTtDemo::LaunchTootipDemoChildDlg(ctx.TtDlgForRectArea,
			_T("IDD_TooltipForRectArea"), IDD_TooltipForRectArea, hdlgMain, &ctx.ptdForRectArea);
		break;

	case IDB_MultilineTooltip:
		CModelessTtDemo::LaunchTootipDemoChildDlg(ctx.TtDlgMultiline,
			_T("IDD_TooltipMultiline"), IDD_TooltipMultiline, hdlgMain, &ctx.ptdMultiline);
		break;

	case IDB_TrackingTooltipConcise:
		CModelessTtDemo::LaunchTootipDemoChildDlg(ctx.TtDlgTrackingTooltip_concise,
			_T("IDD_TrackingTooltip1"), IDD_TrackingConcise, hdlgMain, &ctx.ptdTrackingTooltipConcise);
		break;

	case IDB_TrackingTooltipMisc:
		CModelessTtDemo::LaunchTootipDemoChildDlg(ctx.TtDlgTrackingTooltip_misc,
			_T("IDD_TrackingTooltipMisc"), IDD_TrackingMisc, hdlgMain, &ctx.ptdTrackingTooltipMisc);
		break;

	case IDB_InplaceSimplest:
		CModelessTtDemo::LaunchTootipDemoChildDlg(ctx.TtDlgInplaceSimplest,
			_T("IDD_InplaceSimplest"), IDD_InplaceSimplest, hdlgMain, &ctx.ptdInplaceSimplest);
		break;

	case IDB_InplaceTooltip:
	{
		BOOL bTrans = 0;
		int pt = GetDlgItemInt(hdlgMain, IDE_FontsizePt, &bTrans, bSigned_TRUE);
		if (pt <= 0)
		{
			pt = 8;
			SetDlgItemInt(hdlgMain, IDE_FontsizePt, pt, bSigned_TRUE);
		}

		LPCDLGTEMPLATE dlgtmpl = LoadAndPatchDialogTemplate(g_hinstExe,
			MAKEINTRESOURCE(IDD_InplaceComplex), pt);
			assert(dlgtmpl);

		CModelessTtDemo::LaunchTootipDemoChildDlg(ctx.TtDlgInplaceComplex,
			_T("IDD_InplaceTooltip"), 0, hdlgMain, &ctx.ptdInplaceComplex,
			dlgtmpl);
		
		delete dlgtmpl;
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
	case IDCB_TooltipTitle:
	{
		if(codeNotify==CBN_SELCHANGE)
		{
			int index = ComboBox_GetCurSel(hwndCtl);
			g_TTI_value = (int)ComboBox_GetItemData(hwndCtl, index);
		}
		break;
	}
	case IDCANCEL:
	{
		// Do nothing, so ESC will not close the main UI.
		// Closing main UI is handled by SC_CLOSE.
		break;
	}
	}}
}

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 40,0, IDCB_TooltipTitle);
	jul->AnchorControl(40,0, 100,0, IDE_TtTitleText);

	jul->AnchorControls(0, 0, 0, 0,
		IDCK_EnableMultiline, IDE_LineWidth, IDS_LineWidth, 
		IDB_MultilineTooltip, IDCK_TTS_NOPREFIX,
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

static void fill_TooltipTitleCombobox(HWND hdlg)
{
	static const Enum2Val_st s2v[] =
	{
		{ _T("No tooltip title"), (CONSTVAL_t)-1 },
		ITC_NAMEPAIR(TTI_NONE),
		ITC_NAMEPAIR(TTI_INFO),
		ITC_NAMEPAIR(TTI_WARNING),
		ITC_NAMEPAIR(TTI_ERROR),
		ITC_NAMEPAIR(TTI_INFO_LARGE),
		ITC_NAMEPAIR(TTI_WARNING_LARGE),
		ITC_NAMEPAIR(TTI_ERROR_LARGE),
	};

	HWND hcbx = GetDlgItem(hdlg, IDCB_TooltipTitle);

	for(int i=0; i<ARRAYSIZE(s2v); i++)
	{ 
		int index = ComboBox_AddString(hcbx, s2v[i].EnumName);
		ComboBox_SetItemData(hcbx, i, s2v[i].ConstVal);
	}

	ComboBox_SetCurSel(hcbx, 0); // "No tooltip title" as default

	Dlgbox_EnableComboboxWideDrop(hdlg);

	SetDlgItemText(hdlg, IDE_TtTitleText, _T("My tooltip title"));
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
	SetDlgItemText(hdlg, IDE_MultilineText, 
		_T("Sample Line 1\r\n")
		_T("Sample Line 1\r\n")
		_T("&Menu item\tWith hot key")
	);
	CheckDlgButton(hdlg, IDCK_TTS_NOPREFIX, TRUE);

	SetDlgItemInt(hdlg, IDE_TttOffsetX,  10, bSigned_TRUE);
	SetDlgItemInt(hdlg, IDE_TttOffsetY, -20, bSigned_TRUE);

	CheckDlgButton(hdlg, IDCK_TTF_TRANSPARENT, TRUE);
	CheckDlgButton(hdlg, IDCK_TTF_TRACK, TRUE);
	CheckDlgButton(hdlg, IDCK_TTF_ABSOLUTE, TRUE);
	CheckDlgButton(hdlg, IDCK_ClientToScreen, TRUE);
	SetDlgItemText(hdlg, IDE_DelayAfterTooltipText, _T("0"));

	SetDlgItemText(hdlg, IDE_FontsizePt, _T("8"));

	fill_TooltipTitleCombobox(hdlg);

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDB_TooltipForUic));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

void Dlg_OnSysCommand(HWND hdlg, UINT cmd, int x, int y)
{
	if (cmd == SC_CLOSE)
	{
		PostQuitMessage(0);
		return;
	}
}

void Dlg_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	_CrtDumpMemoryLeaks();
}


INT_PTR WINAPI UserDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	DlgPrivate_st &ctx = *(DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (uMsg) 
	{{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
		HANDLE_dlgMSG(hdlg, WM_RBUTTONDOWN,   Dlg_OnRButtonDown);

		HANDLE_MSG(hdlg, WM_SYSCOMMAND, Dlg_OnSysCommand); // not HANDLE_dlgMSG
	}}
	return FALSE;
}

#define MY_CHECK_MODELESS_DLGMSG(who) \
		if((who) && IsDialogMessage((who)->GetHdlg(), &msg)) continue;


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

		// If some demo dlg needs Tab-key switching focus, we should Check IsDialogMessage() for it.
		// If we desire ESC-key closing the dlg, we also need to check IsDialogMessage().
		//
		MY_CHECK_MODELESS_DLGMSG(ctx.ptdForUic);
		MY_CHECK_MODELESS_DLGMSG(ctx.ptdForRectArea);
		MY_CHECK_MODELESS_DLGMSG(ctx.ptdMultiline);
		MY_CHECK_MODELESS_DLGMSG(ctx.ptdTrackingTooltipConcise);
		MY_CHECK_MODELESS_DLGMSG(ctx.ptdTrackingTooltipMisc);
		MY_CHECK_MODELESS_DLGMSG(ctx.ptdInplaceSimplest);
		MY_CHECK_MODELESS_DLGMSG(ctx.ptdInplaceComplex);

		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	return 0;
}
