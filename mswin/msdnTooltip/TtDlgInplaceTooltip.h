#include "shareinc.h"

#include <mswin/JULayout2.h>
#include <mswin/win32cozy.h>

#include "resource.h"
#include "ModelessTtdemo.h"

//enum { OffScreenX = -32000, OffScreenY = -32000 };

enum { TooltipText_Max = 200 };

class CTtDlgInplaceComplex : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet=nullptr) override;

	virtual void DlgClosing() override;

	TCHAR* my_FeedTooltipText(HWND hdlg, HWND hwndLabel);

	TCHAR m_szTooltip[TooltipText_Max];

private:
	static const int sar_OptUic[];

private:
	static const int sar_UicsToReveal[];
	static const UINT_PTR s_timerId = 1;
};

#ifdef TtDlgInplaceTooltip_IMPL

//
HWND CreateInplaceTooltip_ForUics(HWND hdlg, const int arUics[], int nUics)
{
	// Create the tooltip. g_hinstExe is the global instance handle.
	HWND hwndTT = CreateWindowEx(
		WS_EX_TOPMOST | flag_WS_EX_TRANSPARENT(),
		TOOLTIPS_CLASS,
		NULL, // window title
		TTS_NOPREFIX | TTS_ALWAYSTIP | flag_TTS_BALLOON(),
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hdlg, // tooltip-window's owner
		NULL, NULL, NULL);

	if (!hwndTT)
		return NULL;

	// Associate Uics to the tooltip.
	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.hwnd = hdlg;
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | ((TTF_TRANSPARENT));
	ti.lpszText = LPSTR_TEXTCALLBACK;

	for (int i = 0; i < nUics; i++)
	{
		ti.uId = (UINT_PTR)GetDlgItem(hdlg, arUics[i]);
		assert(ti.uId);

		// Associate this Uic to the tooltip.
		LRESULT succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);

		TCHAR szprefix[40];
		_sntprintf_s(szprefix, _TRUNCATE, _T("CreateInplaceTooltip_ForUics() #%d"), i+1);
		dbg_TTM_ADDTOOL(szprefix, ti, (BOOL)succ);
	
		// Make the tooltip appear immediately (10ms), instead of delaying 500ms.
		SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_INITIAL, 10);
	}
	
	return hwndTT;
}

void RefreshUI_Datetime(HWND hdlg)
{
	SYSTEMTIME st = {};
	GetLocalTime(&st);

	TCHAR shortDate[80], longDate[80];
	TCHAR szTime[80];

	GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st,
		NULL, // use locale format
		shortDate,
		ARRAYSIZE(shortDate)); 
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st,
		NULL, // use locale format
		longDate,
		ARRAYSIZE(shortDate));

	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st,
		NULL, // use locale format
		szTime,
		ARRAYSIZE(szTime));

	int slen = (int)(_tcslen(shortDate) + _tcslen(szTime));
	vaSetDlgItemText(hdlg, IDL_ShortDate, _T("%s / %s (%d TCHARs)"),
		shortDate, szTime, slen);

	slen = (int)(_tcslen(longDate) + _tcslen(szTime));
	vaSetDlgItemText(hdlg, IDL_LongDate, _T("%s / %s (%d TCHARs)"), 
		longDate, szTime, slen);
}

const int CTtDlgInplaceComplex::sar_OptUic[] = { IDS_FontsizePt, IDE_FontsizePt };

const int CTtDlgInplaceComplex::sar_UicsToReveal[] = {
	IDL_ShortDate, IDL_LongDate };

static void enable_Julayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg, 130, 0, 32000, 600);

	jul->AnchorControl(0, 0, 100, 100, IDC_STATIC1);

	jul->AnchorControl(0, 100, 100, 100, IDL_ShortDate);
	jul->AnchorControl(0, 100, 100, 100, IDL_LongDate);

}

TCHAR* CTtDlgInplaceComplex::my_FeedTooltipText(HWND hdlg, HWND hwndLabel)
{
	// In response to TTN_NEEDTEXT:
	// This function's return value will be passed to NMTTDISPINFO.lpszText .
	// * If label text is truncated, the label text in m_szTooltip is returned.
	// * If label text not truncated, NULL is return, so Windows will not show the tooltip.

	// Input parameter note:
	// [hdlg & hwndLabel] collectively identify a hottool already associated with m_hwndTooltip.

	TCHAR wndclass_name[40] = {};
	GetClassName(hwndLabel, wndclass_name, ARRAYSIZE(wndclass_name));
	if (_tcsicmp(wndclass_name, _T("Static")) != 0)
	{
		vaDbgTs(_T("In TTN_NEEDTEXT, Got unexpected wndclass_name(\"%s\")"), wndclass_name);
		return false;
	}

	// Get label text for display-extent calculation.
	// Note: Even for a label with SS_ENDELLIPSIS, the real text is retrieved.

	TCHAR szLabel[TooltipText_Max];
	GetWindowText(hwndLabel, szLabel, ARRAYSIZE(szLabel));
	int lenLabel = (int)_tcslen(szLabel);

	// Get label's required display extent
	HDC hdc = GetDC(hwndLabel);
	HFONT hfont = GetWindowFont(hwndLabel);
	SelectFont(hdc, hfont);

	SIZE size_req = {};
	GetTextExtentPoint32(hdc, szLabel, lenLabel, &size_req);

	ReleaseDC(hwndLabel, hdc);

	RECT rc_label = {};
	GetWindowRect(hwndLabel, &rc_label);
	int label_width = rc_label.right - rc_label.left;

	// Check whether display extent overflows

	if (size_req.cx > label_width)
	{
		// tell tooltip to use same font as our label, must
		SetWindowFont(m_hwndTooltip, hfont, FALSE); 

		vaDbgTs(_T("TTN_NEEDTEXT: Text-extent > label-width (%d > %d), allow tooltip."), size_req.cx, label_width);

		GetWindowText(hwndLabel, m_szTooltip, TooltipText_Max);
		return m_szTooltip;
	}
	else
	{
		vaDbgTs(_T("TTN_NEEDTEXT: Text-extent < label-width (%d < %d), deny tooltip."), size_req.cx, label_width);
		return NULL;
	}
}

bool my_RepositionTooltipOnLabel(HWND hdlg, HWND hwndLabel, HWND hwndTooltip)
{
	RECT rc_label = {};
	GetWindowRect(hwndLabel, &rc_label);

	RECT rc_tt = {};
	GetWindowRect(hwndTooltip, &rc_tt);
	int ttwidth = rc_tt.right - rc_tt.left;
	int ttheight = rc_tt.bottom - rc_tt.top;

	// re-mean rc_label, to reflect in-place pre-adjust tooltip rect (vaDbgTs use)
	rc_label.right = rc_label.left + ttwidth;
	rc_label.bottom = rc_label.top + ttheight;
	rc_tt = rc_label;

	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.hwnd = hdlg;
	ti.uId = (UINT_PTR)hwndLabel;

	SendMessage(hwndTooltip, TTM_ADJUSTRECT, TRUE, (LPARAM)&rc_tt);
	// -- This makes rc_tt a bit large surrounding old rect-area. 
	// On a Win7 1280x1024 screen:
	// TTM_ADJUSTRECT: LT(547,545)RB(748,563)[201*18]
	//              -> LT(541,543)RB(753,564)[212*21]

	SetWindowPos(hwndTooltip, NULL,
		rc_tt.left, rc_tt.top, 0, 0,
		SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	TCHAR rctext1[40], rctext2[40];
	vaDbgTs(_T("TTN_SHOW: TTM_ADJUSTRECT: %s -> %s"),
		RECTtext(rc_label, rctext1), RECTtext(rc_tt, rctext2));

	return true;
}

CModelessChild::Actioned_et
CTtDlgInplaceComplex::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
{
	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

	if (uMsg == WM_INITDIALOG)
	{
		enable_Julayout(m_hdlgMe);

		SetDlgItemText(m_hdlgMe, IDC_STATIC1,
			_T("To see datetime text in full, either hover your mouse over the text label ")
			_T("to activate in-place tooltip, or, resize the dialogbox.")
			);

		// Create the tooltip window.

		m_hwndTooltip = CreateInplaceTooltip_ForUics(m_hdlgMe,
			sar_UicsToReveal, ARRAYSIZE(sar_UicsToReveal));

		vaDbgTs(_T("In %s, created tooltip-hwnd=0x%08X."), msz_name, m_hwndTooltip);

		// Refresh UI datetime
		RefreshUI_Datetime(m_hdlgMe);
		SetTimer(m_hdlgMe, s_timerId, 1000, NULL);

		Enable_Uics(FALSE, m_hdlgParent, sar_OptUic);

		SetFocus(NULL);
		*pMsgRet = AcceptDefaultFocus_FALSE;
		return Actioned_yes;
	}
	else if (uMsg == WM_TIMER)
	{
		if (wParam == s_timerId)
		{
			RefreshUI_Datetime(m_hdlgMe);
		}
		return Actioned_yes;
	}
	else if (uMsg == WM_NOTIFY)
	{
		dbg_WM_NOTIFY(wParam, lParam);

		NMHDR *pnmh = (NMHDR *)lParam;
		HWND hwndLabel = (HWND)pnmh->idFrom;

		if (pnmh->code == TTN_NEEDTEXT)
		{
			// Prepare dynamic text for the tooltip.
			// Note about output pdi->lpszText : 
			// * If we set to non-NULL, TTN_SHOW will follow.
			// * If we set to NULL, Windows will not send use TTN_SHOW and 
			//   the tooltip will not show this time.

			NMTTDISPINFO *pdi = (NMTTDISPINFO *)pnmh;
			pdi->lpszText = my_FeedTooltipText(m_hdlgMe, hwndLabel);
		}
		else if (pnmh->code == TTN_SHOW)
		{
			bool isok = my_RepositionTooltipOnLabel(m_hdlgMe, hwndLabel, m_hwndTooltip);
			if (!isok)
				return Actioned_no;

			*pMsgRet = TRUE;
			// -- Important, as return-value for TTN_SHOW. It tells tooltip code
			// to respect our SetWindowPos().
			return Actioned_yes;
		}

		return Actioned_no;
	}
	if (uMsg == WM_COMMAND)
	{
		return Actioned_no;
	}
	else 
		return actioned;
}


void CTtDlgInplaceComplex::DlgClosing()
{
	__super::DlgClosing();

	Enable_Uics(TRUE, m_hdlgParent, sar_OptUic);

	KillTimer(m_hdlgMe, s_timerId);
}


#endif // TtDlgInplaceTooltip_IMPL
