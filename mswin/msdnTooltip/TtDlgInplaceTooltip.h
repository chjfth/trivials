#include "shareinc.h"

#include <mswin/JULayout2.h>
#include <mswin/win32cozy.h>

#include "resource.h"
#include "ModelessTtdemo.h"

enum { OffScreenX = -32000, OffScreenY = -32000 };

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
	HWND hwndTT = CreateWindowEx(
		NULL, // better to assign WS_EX_TOPMOST here.
		TOOLTIPS_CLASS,
		NULL, // window title
		WS_POPUP | TTS_ALWAYSTIP,
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
	ti.lpszText = _T("Uic-init tooltip text");

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

const int CTtDlgInplaceTooltip::sar_Uics[] = {
	IDL_ShortDate, IDL_LongDate };

static void enable_Julayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg, 130, 0, 32000, 600);

	jul->AnchorControl(0, 0, 100, 100, IDC_STATIC1);

	jul->AnchorControl(0, 100, 100, 100, IDL_ShortDate);
	jul->AnchorControl(0, 100, 100, 100, IDL_LongDate);

}

CModelessChild::Actioned_et
CTtDlgInplaceTooltip::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
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
			sar_Uics, ARRAYSIZE(sar_Uics));

		vaDbgTs(_T("In %s, created tooltip-hwnd=0x%08X."), msz_name, m_hwndTooltip);

		// Refresh UI datetime
		RefreshUI_Datetime(m_hdlgMe);
		SetTimer(m_hdlgMe, s_timerId, 1000, NULL);

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
			
			// let tooltip use the same font as target label. 
			SetWindowFont(m_hwndTooltip, hfont, FALSE);


			SIZE size_req = {};
			GetTextExtentPoint32(hdc, szLabel, lenLabel, &size_req);

			ReleaseDC(hwndLabel, hdc);

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

				vaDbgTs(_T("Text-extent > label-width (%d > %d), will show inplace-tootlip."), size_req.cx, label_width);

				ti.lpszText = szLabel;
				SendMessage(m_hwndTooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);

				// And reposition the tooltip "in-place".
				RECT rc_label_ext = {};
				GetWindowRect(hwndLabel, &rc_label_ext);
				rc_label_ext.right = rc_label_ext.left + size_req.cx;
				rc_label_ext.bottom = rc_label_ext.top + size_req.cy;

				SendMessage(m_hwndTooltip, TTM_ADJUSTRECT, TRUE, (LPARAM)&rc_label_ext);
				// -- This make rc_label_ext nudge a bit towards left and top. 
				// On a Win7 1280x1024 screen:
				// From {LT(551, 545) RB(773, 558)  [222 x 13]}
				//   to {LT(545, 543) RB(778, 559)  [233 x 16]}

				SetWindowPos(m_hwndTooltip, NULL, 
					rc_label_ext.left, rc_label_ext.top, RECTwidth(rc_label_ext), RECTheight(rc_label_ext), 
					SWP_NOZORDER | SWP_NOACTIVATE);	
				// -- Must not include SWP_NOSIZE, bcz tooltip's width may have changed 
				// due to new text, and the new size has been calculated in rc_label_ext .
			}
			else
			{
				// No. We will "suppress" the tooltip by moving it off-screen.

				vaDbgTs(_T("Text-extent < label-width (%d < %d), will hide inplace-tootlip."), size_req.cx, label_width);

				SetWindowPos(m_hwndTooltip, NULL,
					OffScreenX, OffScreenY, 0, 0,
					SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

				// ti.lpszText = _T(""); // wrong way
				// -- NOTE: Don't set empty text, which will cause later TTN_SHOW to never come up.
			}

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


void CTtDlgInplaceTooltip::DlgClosing()
{
	__super::DlgClosing();

	KillTimer(m_hdlgMe, s_timerId);
}


#endif // TtDlgInplaceTooltip_IMPL
