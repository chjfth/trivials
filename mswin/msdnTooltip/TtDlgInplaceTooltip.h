#include "shareinc.h"

#include "resource.h"
#include "ModelessTtdemo.h"

class CTtDlgInplaceTooltip : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet=nullptr) override;

	virtual void DlgClosing() override;

private:
	static const int sar_Uics[];
	UINT_PTR s_timerId = 1;

private:

};

#ifdef TtDlgInplaceTooltip_IMPL

//
HWND CreateInplaceTooltip_ForUics(HWND hdlg, const int arUics[], int nUics)
{
	// Create the tooltip. g_hinstExe is the global instance handle.
	HWND hwndTip = CreateWindowEx(
		NULL, // better to assign WS_EX_TOPMOST here.
		TOOLTIPS_CLASS,
		NULL, // window title
		WS_POPUP | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hdlg, // tooltip-window's owner
		NULL, NULL, NULL);

	if (!hwndTip)
		return NULL;

	// Associate Uics to the tooltip.
	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.hwnd = hdlg;
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | ((TTF_TRANSPARENT));
	ti.lpszText = _T("Uic-init tooltip text");

	for (int i = 0; i < nUics; i++)
	{
		ti.uId = (UINT_PTR)GetDlgItem(hdlg, arUics[i]);
		assert(ti.uId);

		// Associate this Uic to the tooltip.
		BOOL succ = SendMessage(hwndTip, TTM_ADDTOOL, 0, (LPARAM)&ti);

		TCHAR szprefix[40];
		_sntprintf_s(szprefix, _TRUNCATE, _T("CreateInplaceTooltip_ForUics() #%d"), i+1);
		dbg_TTM_ADDTOOL(szprefix, ti, succ);
	}
	

	return hwndTip;
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

const int CTtDlgInplaceTooltip::sar_Uics[] = {
	IDL_ShortDate, IDL_LongDate };

CModelessChild::Actioned_et
CTtDlgInplaceTooltip::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
{
	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

	if (uMsg == WM_INITDIALOG)
	{
		SetDlgItemText(m_hdlgMe, IDC_STATIC1,
			_T("To see datetime text in full, either hover your mouse over the text label ")
			_T("to activate in-place tooltip, or, resize the dialogbox.")
			);

		// Create the tooltip window.

		m_hwndTooltip = CreateInplaceTooltip_ForUics(m_hdlgMe,
			sar_Uics, ARRAYSIZE(sar_Uics));

		vaDbgTs(_T("In %s, created tooltip-hwnd=0x%08X."), msz_name, m_hwndTooltip);

		// Refresh UI datetime
		RefreshUI_Datetime(m_hdlgMe);
		SetTimer(m_hdlgMe, s_timerId, 1000, NULL);

		SetFocus(NULL);
		*pMsgRet = AcceptDefaultFocus_FALSE;
		return Actioned_yes;
	}
	if (uMsg == WM_TIMER)
	{
		if (wParam == s_timerId)
		{
			RefreshUI_Datetime(m_hdlgMe);
		}
		return Actioned_yes;
	}
	if (uMsg == WM_NOTIFY)
	{
		dbg_WM_NOTIFY(wParam, lParam);

		// Here we determine whether to pop-up the in-place tooltip.

		NMHDR *pnmh = (NMHDR *)lParam;
		if (pnmh->code == TTN_SHOW)
		{
//			int uic = (int)(pnmh->hwndFrom);
//			HWND hwndLabel = GetDlgItem(m_hdlgMe, uic);
			HWND hwndLabel = (HWND)(pnmh->idFrom);

			TCHAR wndclass_name[40] = {};
			GetClassName(hwndLabel, wndclass_name, ARRAYSIZE(wndclass_name));
			if (_tcsicmp(wndclass_name, _T("Static")) != 0)
			{
				vaDbgTs(_T("In TTN_SHOW, Got unexpected wndclass_name(\"%s\")"), wndclass_name);
				return Actioned_no;
			}

			// Get text and length.
			// Note: Even for a label with SS_ENDELLIPSIS, the real text is retrieved.
			TCHAR szLabel[80];
			GetWindowText(hwndLabel, szLabel, ARRAYSIZE(szLabel));
			int lenLabel = (int)_tcslen(szLabel);

			// Get text label required display extent
			HDC hdc = GetDC(hwndLabel);
			HFONT hfont = GetWindowFont(hwndLabel);
			SelectFont(hdc, hfont);
			
			SIZE size_req = {};
			GetTextExtentPoint32(hdc, szLabel, lenLabel, &size_req);

			RECT rc_label = {};
			GetClientRect(hwndLabel, &rc_label);
			int label_width = rc_label.right - rc_label.left;

			// Check whether display extent overflows
			TOOLINFO ti = { sizeof(TOOLINFO) };
			ti.hwnd = m_hdlgMe;
			ti.uId = pnmh->idFrom;
			if (size_req.cx > label_width)
			{
				// Yes, need update text.
				ti.lpszText = szLabel;
				SendMessage(m_hwndTooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);

				// And reposition the tooltip "in-place".
				GetWindowRect(hwndLabel, &rc_label);

				SendMessage(m_hwndTooltip, TTM_ADJUSTRECT, TRUE, (LPARAM)&rc_label);
				// -- This make rc_label nudge a bit towards left and top. For example:
				// From {LT(97, 172) RB(274, 185)[177 x 13]}
				//   to {LT(91, 170) RB(279, 186)[188 x 16]}

				SetWindowPos(m_hwndTooltip, NULL, 
					rc_label.left, rc_label.top, 0, 0, 
					SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
				
				*pMsgRet = TRUE; 
				// -- Important, as return-value for TTN_SHOW. It tells tooltip code
				// to respect our SetWindowPos().
			}
			else
			{
				// No. Set empty text, so the tooltip does not show-up.
				ti.lpszText = _T("");
				SendMessage(m_hwndTooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
			}

			ReleaseDC(hwndLabel, hdc);
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


void CTtDlgInplaceTooltip::DlgClosing()
{
	__super::DlgClosing();

	KillTimer(m_hdlgMe, s_timerId);
}


#endif // TtDlgInplaceTooltip_IMPL
