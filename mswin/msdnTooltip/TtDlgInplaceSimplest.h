#include "shareinc.h"

#include "resource.h"
#include "ModelessTtdemo.h"

class CTtDlgInplaceSimplest : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet=nullptr) override;

private:
	void enable_Julayout();
};

#ifdef TtDlgInplaceSimplest_IMPL

HWND CreateToolTip_InplaceSimplest(HWND hwndOwner, int uicHottool)
{
	// Important note: To avoid in-place tooltip flicking, 
	// We need either WS_EX_TRANSPARENT or TTF_TRANSPARENT.

	// Note for Static label as hottool, the Static uic needs SS_NOTIFY flag,
	// so that it can receive mouse messages.

	HWND hwndUic = GetDlgItem(hwndOwner, uicHottool);

	// Create a tooltip.
	HWND hwndTT = CreateWindowEx(
		WS_EX_TOPMOST | flag_WS_EX_TRANSPARENT(),
		TOOLTIPS_CLASS,
		NULL, // window title
		TTS_NOPREFIX | TTS_ALWAYSTIP | flag_TTS_BALLOON(),
		CW_USEDEFAULT, CW_USEDEFAULT, 
		CW_USEDEFAULT, CW_USEDEFAULT,
		hwndOwner, 
		NULL, 
		NULL, // g_hinstExe, using NULL is OK.
		NULL);

	// Set up "tool" information. In this case, the "tool" is the entire parent window.

	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | ((TTF_TRANSPARENT));
	ti.uFlags |= flag_TTF_CENTERTIP();
	ti.hwnd = hwndOwner;
	ti.uId = (UINT_PTR)hwndUic;
	
	// Set a static tooltip text, bcz we are "simplest".
	// ti.lpszText must NOT be NULL, otherwise, TTN_SHOW will not come up.
	TCHAR szText[200] = {};
	GetWindowText(hwndUic, szText, ARRAYSIZE(szText));
	ti.lpszText = szText;

	// Associate the tooltip with the "tool" window.
	LRESULT succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
	assert(succ);

	dbg_TTM_ADDTOOL(_T("CreateToolTip_InplaceSimplest()"), ti, (BOOL)succ);

	// Make the tooltip appear immediately (10ms), instead of delaying 500ms.
	SendMessage(hwndTT, TTM_SETDELAYTIME, TTDT_INITIAL, 10);

	ui_TooltipAddtitle(hwndOwner, hwndTT);

	return hwndTT;
}

void CTtDlgInplaceSimplest::enable_Julayout()
{
	JULayout *jul = JULayout::EnableJULayout(m_hdlgMe, 0, 0, 32000, 1600);

	jul->AnchorControl(0,0, 0,100, IDC_STATIC1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT1);
	jul->AnchorControl(50,100, 50,100, IDOK);
}


CModelessChild::Actioned_et
CTtDlgInplaceSimplest::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
{
	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

	TCHAR szRectLabel[80], szRectTooltip[80];
	HWND hwndLabel = GetDlgItem(m_hdlgMe, IDC_STATIC1);

	if (uMsg == WM_INITDIALOG)
	{
		// Create the tooltip window.

		enable_Julayout();

		m_hwndTooltip = CreateToolTip_InplaceSimplest(m_hdlgMe, IDC_STATIC1);

		vaDbgTs(_T("In %s, created tooltip-hwnd=0x%08X."), msz_name, m_hwndTooltip);
		
		RECT rcsLabel = {}; // will store screen coordinate of the label
		GetWindowRect(hwndLabel, &rcsLabel);
		vaSetDlgItemText(m_hdlgMe, IDC_EDIT1, _T("Label: %s"), RECTtext(rcsLabel, szRectLabel));

		SetFocus(GetDlgItem(m_hdlgMe, IDOK));
		*pMsgRet = AcceptDefaultFocus_FALSE;
		return Actioned_yes;
	}
	else if (uMsg == WM_NOTIFY)
	{
		dbg_WM_NOTIFY(wParam, lParam);

		NMHDR *pnmh = (NMHDR *)lParam;
		if (pnmh->code == TTN_SHOW)
		{
			HWND hwndLabel = GetDlgItem(m_hdlgMe, IDC_STATIC1);
			assert((UINT_PTR)hwndLabel == pnmh->idFrom);

			// Tell tooltip to use the same font as the static label.
			HFONT hfont = GetWindowFont(hwndLabel);
			SetWindowFont(m_hwndTooltip, hfont, FALSE);

			RECT rcsLabel = {}; // will store screen coordinate of the label
			GetWindowRect(hwndLabel, &rcsLabel);

			RECT rcsTooltip = rcsLabel; // will adjust it immediately
			SendMessage(m_hwndTooltip, TTM_ADJUSTRECT, TRUE, (LPARAM)&rcsTooltip);
			SetWindowPos(m_hwndTooltip, 0, 
				rcsTooltip.left, rcsTooltip.top, 0, 0,
				SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE
				);

			vaSetDlgItemText(m_hdlgMe, IDC_EDIT1,
				_T("Label: %s\r\n")
				_T("Tooltip: %s"),
				RECTtext(rcsLabel, szRectLabel),
				RECTtext(rcsTooltip, szRectTooltip));

			*pMsgRet = TRUE;
			// -- Important, as return-value for TTN_SHOW. It tells tooltip code
			// to respect our SetWindowPos().
			return Actioned_yes;
		}

		return Actioned_no;
	}
	
	else
		return actioned;
}

#endif // TtDlgInplaceSimplest_IMPL
