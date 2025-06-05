#include "shareinc.h"

#include <mswin/JULayout2.h>
#include <mswin/win32cozy.h>

#include "resource.h"
#include "ModelessTtdemo.h"

enum { OffScreenX = -32000, OffScreenY = -32000 };

enum { TooltipText_Max = 200 };

class CTtDlgInplaceComplex : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet=nullptr) override;

	virtual void DlgClosing() override;

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

bool my_RepositionTooltipOnLabel(HWND hdlg, HWND hwndLabel, HWND hwndTooltip)
{
	// This function checks whether static-label's text is truncated,
	// * if truncated, hwndTooltip will be repositioned over the label to reveal its full text.
	// * if not truncated, hwndTooltip will be placed off-screen (become invisible to user).
	//

	// Input parameter requirement:
	// [hdlg & hwndLabel] collectively identify a hottool already associated with hwndTooltip.

	TCHAR wndclass_name[40] = {};
	GetClassName(hwndLabel, wndclass_name, ARRAYSIZE(wndclass_name));
	if (_tcsicmp(wndclass_name, _T("Static")) != 0)
	{
		vaDbgTs(_T("In TTN_SHOW, Got unexpected wndclass_name(\"%s\")"), wndclass_name);
		return false;
	}

	// Get label text and length.
	// Note: Even for a label with SS_ENDELLIPSIS, the real text is retrieved.
	TCHAR szLabel[80];
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
		// Yes, need update text.

		TOOLINFO ti = { sizeof(TOOLINFO) };
		ti.hwnd = hdlg;
		ti.uId = (UINT_PTR)hwndLabel;

		// And reposition the tooltip "in-place".
		rc_label.right = rc_label.left + size_req.cx;
		rc_label.bottom = rc_label.top + size_req.cy;
		RECT rc_label_ext = rc_label;

		SendMessage(hwndTooltip, TTM_ADJUSTRECT, TRUE, (LPARAM)&rc_label_ext);
		// -- This make rc_label_ext nudge a bit towards left and top. 
		// On a Win7 1280x1024 screen:
		// TTM_ADJUSTRECT: LT(547,545)RB(735,558)[188*13] 
		//              -> LT(541,543)RB(740,559)[199*16]

		SetWindowPos(hwndTooltip, NULL,
			rc_label_ext.left, rc_label_ext.top, 0, 0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

		TCHAR rctext1[40], rctext2[40];
		vaDbgTs(_T("Text-extent > label-width (%d > %d). TTM_ADJUSTRECT: %s -> %s"),
			size_req.cx, label_width,
			RECTtext(rc_label, rctext1), RECTtext(rc_label_ext, rctext2));
	}
	else
	{
		// No. We will "suppress" the tooltip by moving it off-screen.

		vaDbgTs(_T("Text-extent < label-width (%d < %d). No show inplace-tootlip."), size_req.cx, label_width);

		SetWindowPos(hwndTooltip, NULL,
			OffScreenX, OffScreenY, 0, 0,
			SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

		// ti.lpszText = _T(""); // wrong way (todo-confirm)
		// -- NOTE: Don't set empty text, which will cause later TTN_SHOW to never come up.
	}
	
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

		// Here we determine whether to pop-up the in-place tooltip.

		NMHDR *pnmh = (NMHDR *)lParam;
		if (pnmh->code == TTN_NEEDTEXT)
		{
			// Memo: TTN_NEEDTEXT comes first, then TTN_SHOW follows.
			
			// Prepare dynamic text for the tooltip.
			HWND hwndLabel = (HWND)pnmh->idFrom;

			GetWindowText(hwndLabel, m_szTooltip, TooltipText_Max);

			NMTTDISPINFO *pdi = (NMTTDISPINFO *)pnmh;
			pdi->lpszText = m_szTooltip;

			// Tell tooltip to use the same font as the static label.
			//
			HFONT hfont = GetWindowFont(hwndLabel);
			SetWindowFont(m_hwndTooltip, hfont, FALSE);
		}
		else if (pnmh->code == TTN_SHOW)
		{
			HWND hwndLabel = (HWND)(pnmh->idFrom);

			bool isok = my_RepositionTooltipOnLabel(m_hdlgMe, hwndLabel, m_hwndTooltip);
			if (!isok)
				return Actioned_no;

			*pMsgRet = TRUE;
			// -- Important, as return-value for TTN_SHOW. It tells tooltip code
			// to respect our SetWindowPos().
		}

		return Actioned_yes;
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
