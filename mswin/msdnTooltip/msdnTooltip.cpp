#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#include <mswin/winuser.itc.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

#define ASSERT_DLG_SUCC(hdlg, dlg_rcid) \
	if((hdlg)==NULL) { \
		vaMsgBox(NULL, MB_OK|MB_ICONERROR, NULL, _T("Main dialog creation fail: ") _T(#dlg_rcid)); \
		exit(4); \
	}


//typedef INT_PTR WINAPI DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

class CTtdemoDialog
{
	// This class helps the parent-dialog manage this object as modeless child-dialog.

public:
	CTtdemoDialog(const TCHAR *name, UINT dlg_resid,
		HWND hdlgParent, CTtdemoDialog** pptrme_by_parent);

	BOOL CreateTheDialog();

	HWND GetHdlg() { return m_hdlgMe; } // debug purpose

protected:
	enum Relay_et { Relay_no=0, Relay_yes=1 };
	virtual Relay_et DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	static INT_PTR WINAPI BaseDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	int m_dlgMe_resid;
	const TCHAR *m_name;
	HWND m_hdlgMe;

	HWND m_hdlgParent;
	CTtdemoDialog* &mr_ptrme_by_parent;

///////////// 
	HWND m_hwndTooltip = NULL;

public:
	template<typename T_TtDemoDlg>
	static BOOL LaunchTootipDemoChildDlg(const TCHAR *name, UINT iddlg_child, HWND hwndParent, CTtdemoDialog **pptd)
	{
		BOOL succ = 0;
		CTtdemoDialog* &ptd = *pptd;
		if (!ptd)
		{
			CTtdemoDialog *ptdnew = new T_TtDemoDlg(name, iddlg_child, hwndParent, &ptd);
			assert(ptdnew == ptd);

			succ = ptd->CreateTheDialog();
			assert(succ);

			return succ ? TRUE : FALSE;
		}
		else
		{
			SetForegroundWindow(ptd->GetHdlg());
			return TRUE;
		}
	}
};

// BOOL CTtdemoDialog::LaunchTootipDemoChildDlg(
// 	const TCHAR *name, UINT iddlg_child, HWND hwndParent, CTtdemoDialog **pptd)


CTtdemoDialog::CTtdemoDialog(const TCHAR *name, UINT dlg_resid,
	HWND hdlgParent, CTtdemoDialog** pptrme_by_parent)
	: mr_ptrme_by_parent(*pptrme_by_parent)
{
	m_name = name;

	m_dlgMe_resid = dlg_resid;
	m_hdlgMe = NULL;

	m_hdlgParent = hdlgParent;
	mr_ptrme_by_parent = this;
}

BOOL CTtdemoDialog::CreateTheDialog()
{
	m_hdlgMe = CreateDialogParam(g_hinstExe, MAKEINTRESOURCE(m_dlgMe_resid),
		m_hdlgParent,
		BaseDlgProc,
		(LPARAM)this // dlgparam
	);
	ASSERT_DLG_SUCC(m_hdlgMe, IDD_TooltipForUic);
	
	return m_hdlgMe ? TRUE : FALSE;
}

INT_PTR WINAPI CTtdemoDialog::BaseDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CTtdemoDialog *pttdlg = (CTtdemoDialog*)GetWindowLongPtr(hdlg, DWLP_USER);

	if(uMsg==WM_INITDIALOG)
	{
		assert(!pttdlg);
		assert(lParam);

		pttdlg = (CTtdemoDialog*)lParam;
		pttdlg->m_hdlgMe = hdlg;

		SetWindowLongPtr(hdlg, DWLP_USER, lParam);
	}

	
	Relay_et is_relay = Relay_yes;
	
	if(pttdlg && uMsg!=0)
	{
		// note: WM_SETFONT precedes WM_INITDIALOG, so we need to check pttdlg.
		// note: After closing child dlg, we could see uMsg==0, weird.

		pttdlg->DlgProc(uMsg, wParam, lParam);
	}
	else
	{
		if(!pttdlg) {
			vaDbgTs(_T("note: We see uMsg=%s when pttdlg is NULL."), ITCSv(uMsg, itc::WM_xxx));
		}
		if(uMsg==0) {
			vaDbgTs(_T("note: We see uMsg==0 in DialogProc."));
		}
	}

	return is_relay ? FALSE : TRUE; 
	// -- TRUE means "let outer-layer/system go on processing this uMsg.
}

CTtdemoDialog::Relay_et
CTtdemoDialog::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg==WM_INITDIALOG)
	{
		vaDbgTs(_T("%s (hdlg=0x%08X) created."), m_name, m_hdlgMe);
		return Relay_no;
	}
	else if(uMsg==WM_COMMAND)
	{
		assert(mr_ptrme_by_parent == this);

		UINT cmd = GET_WM_COMMAND_ID(wParam, lParam);
		if(cmd==IDCANCEL)
		{
			assert(IsWindow(m_hdlgMe));

			SetWindowLongPtr(m_hdlgMe, DWLP_USER, NULL);

			if (m_hwndTooltip)
			{
				vaDbgTs(_T("Tooltip(hwnd 0x%08X) destroy."), m_hwndTooltip);
				DestroyWindow(m_hwndTooltip);
				m_hwndTooltip = NULL;
			}

			vaDbgTs(_T("%s (hdlg=0x%08X) EndDialog()."), m_name, m_hdlgMe);

			EndDialog(m_hdlgMe, 0);

			// Tell parent that I have destroyed myself.
			mr_ptrme_by_parent = NULL;
			delete this;
		}
		return Relay_no;
	}
	else if(uMsg==WM_DESTROY)
	{
		// [2025-05-10] Q: Why I can not see this?
		// Neither in debug-message of BaseDlgProc().
		return Relay_yes;
	}
	else
		return Relay_yes;
}

// Description:
//   Creates a tooltip for an item in a dialog box.
// Parameters:
//   idTool - identifier of an dialog box item.
//   nDlg - window handle of the dialog box.
//   pszText - string to use as the tooltip text.
// Returns:
//   The handle to the tooltip.
//
HWND CreateToolTip_ForUic(int toolID, HWND hDlg, PCTSTR pszText)
{
	if (!toolID || !hDlg || !pszText)
	{
		return FALSE;
	}
	// Get the window of the tool.
	HWND hwndTool = GetDlgItem(hDlg, toolID);

	// Create the tooltip. g_hInst is the global instance handle.
	HWND hwndTip = CreateWindowEx(NULL, 
		TOOLTIPS_CLASS, 
		NULL, // window title
		WS_POPUP |TTS_ALWAYSTIP | TTS_BALLOON,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hDlg, // tooltip-window's owner
		NULL, 
		g_hinstExe, NULL);

	if (!hwndTool || !hwndTip)
	{
		return (HWND)NULL;
	}                             

	// Associate the tooltip with the tool.
	TOOLINFO toolInfo = { sizeof(TOOLINFO) };
	toolInfo.hwnd = hDlg;
	toolInfo.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	toolInfo.uId = (UINT_PTR)hwndTool;
	toolInfo.lpszText = (PTSTR)pszText; // I believe tooltip internal code will make a string copy.
	SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&toolInfo);

	return hwndTip;
}

class CTtDlgForUic : public CTtdemoDialog
{
public:
	using CTtdemoDialog::CTtdemoDialog; // this requires C++11, VC2015+

	Relay_et DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

CTtdemoDialog::Relay_et
CTtDlgForUic::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CTtdemoDialog::DlgProc(uMsg, wParam, lParam);

	if(uMsg==WM_INITDIALOG)
	{
		// Create the tooltip window.

		m_hwndTooltip = CreateToolTip_ForUic(IDB_HasTooltip, m_hdlgMe, 
			_T("This is the tooltip for the button IDB_HasTootip"));

		vaDbgTs(_T("Called CreateToolTip_ForUic(), tooltip-hwnd=0x%08X."), m_hwndTooltip);
	}

	return Relay_no;
}


//////////////////////////////////////////////////////////////////////////

HWND CreateToolTipForRectArea(HWND hwndOwner, PCTSTR pszText)
{
	// Create a tooltip.
	HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, 
		TOOLTIPS_CLASS, 
		NULL, // window title
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hwndOwner, NULL, g_hinstExe, NULL);

	SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	// Set up "tool" information. In this case, the "tool" is the entire parent window.

	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hwndOwner;
	GetClientRect(hwndOwner, &ti.rect);
	ti.hinst = g_hinstExe; // required?
	ti.lpszText = (PTSTR)pszText; // I believe tooltip internal code will make a string copy.

	// Associate the tooltip with the "tool" window.
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);

	return hwndTT;
}

class CTtDlgForRectArea : public CTtdemoDialog
{
public:
	using CTtdemoDialog::CTtdemoDialog; // this requires C++11, VC2015+

	Relay_et DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

CTtdemoDialog::Relay_et
CTtDlgForRectArea::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CTtdemoDialog::DlgProc(uMsg, wParam, lParam);

	if (uMsg == WM_INITDIALOG)
	{
		// Create the tooltip window.

		m_hwndTooltip = CreateToolTipForRectArea(m_hdlgMe,
			_T("This is tooltip for a whole client-area."));

		vaDbgTs(_T("Called CreateToolTipForRectArea(), tooltip-hwnd=0x%08X."), m_hwndTooltip);
	}

	return Relay_no;
}

//////////////////////////////////////////////////////////////////////////

struct DlgPrivate_st
{
	int clicks;
	CTtdemoDialog *ptdForUic; // HWND hdlgTtForUic;
	CTtdemoDialog *ptdForRectArea; // HWND hdlgTtForRectArea;
};



void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	HWND hdlgMain = hdlg;
	DlgPrivate_st &ctx = *(DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDB_TooltipForUic:
		CTtdemoDialog::LaunchTootipDemoChildDlg<CTtDlgForUic>(
			_T("IDD_TooltipForUic"), IDD_TooltipForUic, hdlgMain, &ctx.ptdForUic);
		break;
	case IDB_TooltipForRectArea:
		CTtdemoDialog::LaunchTootipDemoChildDlg<CTtDlgForRectArea>(
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

	// ShowWindow(hdlgMain, nCmdShow); // not necc bcz of WS_VISIBLE in .rc

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
