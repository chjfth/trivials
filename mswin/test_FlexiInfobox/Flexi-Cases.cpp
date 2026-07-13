#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <time.h>

#include <ps_TCHAR.h>
#include <commdefs.h>
#include <snTprintf.h>
#include <snTcat.h>
#include <EnsureClnup_mswin.h>
#include <mswin/FlexiInfobox.h>
#include <mswin/FlexiInfobox_ids.h>

#include "resource.h"

extern HINSTANCE g_hinst;


typedef FIB_ret (*PROC_DoCase)(HWND hwnd, int idHuman, LPCTSTR ptext);

struct Case_st
{
	PROC_DoCase func;
	LPCTSTR text;
	int idHuman; // one-based, no skip
};

FIB_ret fcSimplest(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	ggt_FlexiInfo(hwnd, ptext);
	return FIB_OK;
}

FIB_ret fcCustomizeTitle(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibInput_st si;
	si.title = _T("Your message"); // !
	si.szBtnOK = _T("Close");
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

const TCHAR *TitleByHumanId(int idHuman)
{
	static TCHAR s_szTitle[80];
	snTprintf(s_szTitle, _T("Case (%d)"), idHuman);
	return s_szTitle;
}

FIB_ret fcWantCancelOnly(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	// A small probleM here. User tabs to the editbox and Enter, it will return IDOK.
	FibInput_st si; si.title = TitleByHumanId(idHuman);

	si.szBtnCancel = _T("Cancel");
	return ggt_FlexiInfobox(hwnd, &si, ptext); 
}

FIB_ret fcOKandCancel(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibInput_st si; si.title = TitleByHumanId(idHuman);

	si.szBtnOK = _T("OK");
	si.szBtnCancel = _T("Cancel");
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcYESandNO(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&YES");    // !
	si.szBtnCancel = _T("&NO"); // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcYESandNO_default_No(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&YES");
	si.szBtnCancel = _T("&NO");
	si.idDefaultFocus = fib_IDC_BTN_CANCEL; // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcUseMonoFont(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("OK");
	si.fixedwidth_font = true; // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcUseBiggerMonoFont(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("OK");
	si.fixedwidth_font = true; // !
	si.fontsize = 12;          // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcUseBiggerDefaultFont(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibInput_st si; si.title = TitleByHumanId(idHuman);
	si.szBtnOK = _T("OK");
	si.fontsize = 12;  // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcCustomizeIcon(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibInput_st si; si.title = TitleByHumanId(idHuman);
	si.szBtnOK = _T("OK");
	si.hIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_NEWLAND)); // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcDelayClose(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&Yes");
	si.szBtnCancel = _T("&No");
	si.msecDelayClose = 1000;
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcUseNoButtons(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	return ggt_vaFlexiInfobox(hwnd, &si, _T("%s\n\n%s"), ptext,
		_T("Use Close nib or keyboard Alt+F4 to close this infobox."));
}

FibCallback_ret fcDenyCancel_GetText(void *ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	if(cb_info.reason==FIBReason_CancelBtn)
		return FIBcb_Fail;
	else
		return FIBcb_OK;
}
FIB_ret fcDenyCancel(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&OK");
	si.szBtnCancel = _T("No &Cancel");
	si.procGetText = fcDenyCancel_GetText;
	return ggt_vaFlexiInfobox(hwnd, &si, _T("%s\n\n%s"), ptext,
		_T("You can only click OK to close this infobox."));
}

FibCallback_ret fcDenyCancelWithPrompt_GetText(void *ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	// textbuf will equal to your input buffer to ggt_FlexiInfobox()
	// bufchars will equal to si.bufchars.
	if(cb_info.reason==FIBReason_CancelBtn)
	{
		snTprintf(textbuf, bufchars, _T("You see, you cannot Cancel it."));
		return FIBcb_FailIcon; // will show a fail-icon
	}
	else
		return FIBcb_OK;
}
//
FIB_ret fcDenyCancelWithPrompt(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	const int bufsize = 400;
	TCHAR mytext[bufsize]= _T("");
	_tcscat_s(mytext, bufsize, ptext);

	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&OK");
	si.szBtnCancel = _T("No &Cancel");
	si.procGetText = fcDenyCancelWithPrompt_GetText;
	si.bufchars = bufsize; // !
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}

struct fcRefreshable_st
{
	int count;
	const TCHAR *ptn;
};
//
FibCallback_ret fcRefreshable_GetText(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	fcRefreshable_st &ctx = *(fcRefreshable_st*)_ctx;

	if(cb_info.reason==FIBReason_RefreshBtn) // !
	{
		if(ctx.count==0)
			textbuf[0] = _T('\0');

		ctx.count++;
		snTcat(textbuf, bufchars, 
			_T("[%2d]%.*s\n"), 
			ctx.count, ctx.count, ctx.ptn);
	}
	return FIBcb_OK;
}
//
FIB_ret fcRefreshable(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	const int bufsize = 5500;
	TCHAR mytext[bufsize]= _T("");
	_tcscat_s(mytext, bufsize, ptext);

	TCHAR stars[100] = {};
	for(int i=0; i<99; i++) stars[i] = _T('*');

	fcRefreshable_st ctx = {0, stars};

	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&OK");
	si.procGetText = fcRefreshable_GetText;
	si.ctxGetText = &ctx;       // !
	si.isShowRefreshBtn = true; // !
	si.bufchars = bufsize;      // !
//	si.isScrollToEnd = true;
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}


FibCallback_ret fcTimedRefresh_GetText(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	(void)cb_info; (void)_ctx;
	SYSTEMTIME st;
	GetLocalTime(&st);
	snTprintf(textbuf, bufchars, 
		_T("Now time %02d:%02d:%02d"),
		st.wHour, st.wMinute, st.wSecond);
	return FIBcb_OK;
}
//
FIB_ret fcTimedRefresh(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	const int bufsize = 50;
	TCHAR mytext[bufsize]= _T("");

	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&OK");
	si.procGetText = fcTimedRefresh_GetText;
	si.msecAutoRefresh = 1000;  // ! unit: millisec
	si.isAutoRefreshNow = true;
	si.bufchars = bufsize;      // !
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}


struct fcCountDownClose_st
{
	DWORD tkmsec_end;
};
//
FibCallback_ret fcCountDownClose_GetText(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	fcCountDownClose_st &ctx = *(fcCountDownClose_st*)_ctx;

	DWORD tkmsec_now = GetTickCount();
	int msec_remain = ctx.tkmsec_end - tkmsec_now;

	if(msec_remain>0)
	{
		snTprintf(textbuf, bufchars, 
			_T("Wait %d seconds before this infobox closes itself."), 
			msec_remain/1000+1);
		return FIBcb_Fail;
	}
	else
		return FIBcb_CloseDlg; // ! This auto-close the infobox
}
//
FIB_ret fcCountDownClose(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	const int bufsize = 80;
	TCHAR mytext[bufsize]= _T("");

	fcCountDownClose_st ctx = { GetTickCount()+5000 };

	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.procGetText = fcCountDownClose_GetText;
	si.ctxGetText = &ctx;
	si.msecAutoRefresh = 500;  // need it to trigger auto-close
	si.isAutoRefreshNow = true;
	si.bufchars = bufsize;
	si.isNarrowTitle = true;          // optional
	si.isForceHideRefreshCtrl = true; // optional
	si.hIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_CLOCK));
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}


enum { fibcmd_DisplayUTC = 1  };
//
struct fcTimeLocalOrUTC_st
{
	bool isShowUTC;
};
//
FibCallback_ret fcTimeLocalOrUTC_GetText(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	bool &isutc = ((fcTimeLocalOrUTC_st*)_ctx)->isShowUTC;

	if(cb_info.reason==FIBReason_UserCmd) // !
	{
		if(cb_info.idUserCmd==fibcmd_DisplayUTC)
		{
			isutc = cb_info.isTickOn ? true : false;
		}
	}

	SYSTEMTIME st;
	if(isutc)
	{
		GetSystemTime(&st);
		snTprintf(textbuf, bufchars, 
			_T("Now UTC %02d:%02d:%02d"),
			st.wHour, st.wMinute, st.wSecond);
	}
	else
	{
		GetLocalTime(&st);
		snTprintf(textbuf, bufchars, 
			_T("Now time %02d:%02d:%02d"),
			st.wHour, st.wMinute, st.wSecond);
	}
	return FIBcb_OK;
}
//
FIB_ret fcTimeLocalOrUTC(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	const int bufsize = 50;
	TCHAR mytext[bufsize]= _T("");

	fcTimeLocalOrUTC_st ctx = {false};
	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&OK");
	si.procGetText = fcTimeLocalOrUTC_GetText;
	si.ctxGetText = &ctx;
	si.msecAutoRefresh = 1000; 
	si.isAutoRefreshNow = true;
	si.bufchars = bufsize;
	// ! Define user cmds activated by right-clicking blank area
	FibUserCmds_st arUserCmds[] =
	{
		{FIBcmd_CopyText, FIBcst_Default, _T("Copy to clipboard")},
		{fibcmd_DisplayUTC, FIBcst_TickOff, _T("Display time in UTC")},
	};
	si.arUserCmds = arUserCmds;
	si.nUserCmds = ARRAYSIZE(arUserCmds);
//	si.secTooltip = 10; // show tooltip for 10 seconds 
	//
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}

struct fcTitleShowTime_st
{
	__int64 uesecLastRefresh;
};
//
FibCallback_ret fcTitleShowTime_GetText(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	(void)cb_info; 
	__int64 &uesecLastRefresh = ((fcTitleShowTime_st*)_ctx)->uesecLastRefresh;

	__int64 uesecNow = _time64(NULL);
	
	if(uesecNow-uesecLastRefresh<5)
		return FIBcb_OK_NoChange;

	uesecLastRefresh = uesecNow;

	struct tm tmnow;
	_localtime64_s(&tmnow, &uesecNow);
 	snTprintf(textbuf, bufchars, 
 		_T("Now local time %02d:%02d:%02d (refresh every 5 seconds)")
 		_T("\n\n(hint: right click blank area to toggle title time stamp)"),
 		tmnow.tm_hour, tmnow.tm_min, tmnow.tm_sec);
	return FIBcb_OK;
}
//
FIB_ret fcTitleShowTime(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	const int bufsize = 200;
	TCHAR mytext[bufsize]= _T("");

	fcTitleShowTime_st ctx = {0};
	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&OK");
	si.procGetText = fcTitleShowTime_GetText;
	si.ctxGetText = &ctx;
	si.msecAutoRefresh = 1000;
	si.isAutoRefreshNow = true;
	si.bufchars = bufsize;
	// Define user cmds activated by right-clicking blank area
	FibUserCmds_st arUserCmds[] =
	{
		{FIBcmd_LastTextTimeOnTitle, FIBcst_TickOn, _T("Title show last update time")},
		{FIBcmd_MenuSeparator}, // only a visual separator
		{FIBcmd_CopyText, FIBcst_Default, _T("Copy to clipboard")},
	};
	si.arUserCmds = arUserCmds;
	si.nUserCmds = ARRAYSIZE(arUserCmds);
//	si.szTooltipText = _T("cutom tooltip text");
	//
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}

FIB_ret fcSwitchAttachDetach(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibUserCmds_st arUserCmds[] =
	{
		{FIBcmd_CopyText, FIBcst_Default, _T("Copy to clipboard")},
		{FIBcmd_DetachFromParent, FIBcst_Default, _T("Detach infobox from parent")},
	};

	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&Apple");
	si.szBtnCancel = _T("&Pear");
	si.arUserCmds = arUserCmds;
	si.nUserCmds = ARRAYSIZE(arUserCmds);
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FibCallback_ret fcSwitchAttachDetach_withTimer_GetText(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	return FIBcb_OK;
}
//
FIB_ret fcSwitchAttachDetach_withTimer(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibUserCmds_st arUserCmds[] =
	{
		{FIBcmd_LastTextTimeOnTitle, FIBcst_TickOn, _T("Title show last update time")},
		{FIBcmd_DetachFromParent, FIBcst_Default, _T("Detach infobox from parent")},
	};

	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&Apple");
	si.szBtnCancel = _T("&Pear");
	si.procGetText = fcSwitchAttachDetach_withTimer_GetText;
	si.msecAutoRefresh = 1000;
	si.isAutoRefreshNow = true;
	si.arUserCmds = arUserCmds;
	si.nUserCmds = ARRAYSIZE(arUserCmds);
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

enum { fibcmd_EnableCancel = 1 } ;
struct fcDenyCancelSoft_st
{
	bool enable_cancel;
};
//
FibCallback_ret fcDenyCancelSoft_GetText(void *ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	bool &en_cancel = ((fcDenyCancelSoft_st*)ctx)->enable_cancel;
	if(cb_info.reason==FIBReason_CancelBtn)
	{
		if(!en_cancel)
		{
			ggt_vaFlexiInfo(cb_info.hDlg, _T("Cancel button cannot be used now."));
			return FIBcb_Fail;
		}
	}
	else if(cb_info.reason==FIBReason_UserCmd) 
	{
		if(cb_info.idUserCmd==fibcmd_EnableCancel)
		{
			if(cb_info.isTickOn)
			{
				SetDlgItemText(cb_info.hDlg, IDCANCEL, _T("&Cancel")); // tweak with hDlg
				en_cancel = true;
			}
			else
			{
				SetDlgItemText(cb_info.hDlg, IDCANCEL, _T("&No cancel")); // tweak with hDlg
				en_cancel = false;
			}
		}
	}
	
	return FIBcb_OK;
}
//
FIB_ret fcDenyCancelSoft(HWND hwnd, int idHuman, LPCTSTR ptext)
{
	FibUserCmds_st arUserCmds[] =
	{
		{fibcmd_EnableCancel, FIBcst_TickOff, _T("Enable Cancel button")},
	};

	FibInput_st si; si.title = TitleByHumanId(idHuman);
	
	si.szBtnOK = _T("&OK");
	si.szBtnCancel = _T("&No Cancel");
	si.procGetText = fcDenyCancelSoft_GetText;
	fcDenyCancelSoft_st ctx = {false};
	si.ctxGetText = &ctx;
	si.arUserCmds = arUserCmds;
	si.nUserCmds = ARRAYSIZE(arUserCmds);

	return ggt_vaFlexiInfobox(hwnd, &si, _T("%s\n\n%s"), ptext,
		_T("Cancel button is now disabled. Use context menu to enable it.\n\n")
		);
}



Case_st gar_FlexiCases[] = 
{
	{ fcSimplest, _T("Simplest") },
	{ fcCustomizeTitle, _T("Customize title and button text") },
	{ fcWantCancelOnly, _T("Want Cancel button only") },
	{ fcOKandCancel, _T("Both OK can Cancel") },
	{ fcYESandNO, _T("Customize button text to YES and NO") },
	{ fcYESandNO_default_No, _T("Set default button to NO") },
	
	{ fcUseMonoFont, _T("Use mono font (fixed-width font)") },
	{ fcUseBiggerMonoFont, _T("Use bigger mono font size") },
	{ fcUseBiggerDefaultFont, _T("Use bigger font size with default fontface") },

	{ fcCustomizeIcon, _T("Customize icon") },

	{ fcDelayClose, _T("Not allow to close infobox or make choice within 1000 milliseconds") },
	{ fcUseNoButtons, _T("Deliberately no buttons") },

	{ NULL }, // a menu item separator

	{ fcDenyCancel, _T("Deny Cancel button's closing infobox") },
	{ fcDenyCancelWithPrompt, _T("Deny Cancel button and customize prompt") },
	{ fcRefreshable, _T("Use Refresh button to add more text") },
	{ fcTimedRefresh, _T("Auto refresh to show clock time") },
	{ fcCountDownClose, _T("Countdown close: Only closeable by program") },
	{ fcTimeLocalOrUTC, _T("Infobox with context menu (right click blank area)") },
	{ fcTitleShowTime, _T("Title shows last text update time") },
	{ fcSwitchAttachDetach, _T("Use context menu to switch between attached/detached infobox") },
	{ fcSwitchAttachDetach_withTimer, _T("Context menu attached/detached and timer") },

	{ fcDenyCancelSoft, _T("Deny Cancel button, but can be enabled") },
};

void do_Cases(HWND hwnd)
{
	CEC_DestroyMenu hmenu = CreatePopupMenu();

	// Initialize case data
	int i=0, idxHuman=1;
	for(; i<ARRAYSIZE(gar_FlexiCases); i++)
	{
		Case_st &thiscase = gar_FlexiCases[i];
		thiscase.idHuman = idxHuman;
		
		int menucmd = i+1; // strictly array_idx+1

		TCHAR szItem[80];
		snTprintf(szItem, ARRAYSIZE(szItem), _T("%d. %s"), thiscase.idHuman, thiscase.text);

		if(thiscase.func)
		{
			AppendMenu(hmenu, MF_STRING, menucmd, szItem);
			idxHuman++;
		}
		else
			AppendMenu(hmenu, MF_SEPARATOR, 0, 0);
	}

	RECT rectBtn;
	GetWindowRect(GetDlgItem(hwnd, IDC_BTN_CASES), &rectBtn);
	int menucmd = TrackPopupMenu(hmenu, TPM_RETURNCMD, rectBtn.left, rectBtn.bottom, 0, hwnd, NULL);
	if(menucmd>0)
	{
		static int s_count = 0;
		bool isShiftDown = GetAsyncKeyState(VK_SHIFT)<0 ? true:false;

		// Call case function:
		FIB_ret fibret = gar_FlexiCases[menucmd-1].func(
			isShiftDown ? NULL : hwnd, 
			gar_FlexiCases[menucmd-1].idHuman,
			gar_FlexiCases[menucmd-1].text
			);
		
		TCHAR hint[40];
		snTprintf(hint, ARRAYSIZE(hint), _T("[count=%d] FIB_ret=%d"), ++s_count, fibret);

		SetDlgItemText(hwnd, IDC_EDIT_HINT, hint);
	}
}
