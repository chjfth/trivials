#include "shareinc.h"

#include "resource.h"
#include "ModelessTtdemo.h"

class CTtDlgForRectArea : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet=nullptr) override;

private:
	void enable_Julayout();
};

#ifdef TtDlgForRectArea_IMPL

HWND CreateToolTipForRectArea(HWND hwndOwner, PCTSTR pszText)
{
	// Create a tooltip.
	HWND hwndTT = CreateWindowEx(
		WS_EX_TOPMOST | flag_WS_EX_TRANSPARENT(),
		TOOLTIPS_CLASS,
		NULL, // window title
		TTS_NOPREFIX | TTS_ALWAYSTIP | flag_TTS_BALLOON(),
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hwndOwner, 
		NULL, 
		NULL, // g_hinstExe, using NULL is OK.
		NULL);

	// Set up "tool" information. In this case, the "tool" is the entire parent window.

	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.uFlags = TTF_SUBCLASS | flag_TTF_CENTERTIP();
	ti.hwnd = hwndOwner;
	GetClientRect(hwndOwner, &ti.rect);

	ti.uId = 0;
	ti.hinst = NULL; // not required to be g_hinstExe, msdn bug!
	
	ti.lpszText = (PTSTR)pszText; // I believe tooltip internal code will make a string copy.

	// Associate the tooltip with the "tool" window.
	LRESULT succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);

	dbg_TTM_ADDTOOL(_T("CreateToolTipForRectArea()"), ti, (BOOL)succ);

	ui_TooltipAddtitle(hwndOwner, hwndTT);

	return hwndTT;
}

void CTtDlgForRectArea::enable_Julayout()
{
	JULayout *jul = JULayout::EnableJULayout(m_hdlgMe, 200, 100, 32000, 600);

	jul->AnchorControl(12,33, 88,67, IDC_STATIC1);
}


CModelessChild::Actioned_et
CTtDlgForRectArea::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
{
	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

	if (uMsg == WM_INITDIALOG)
	{
		enable_Julayout();

		// Create the tooltip window.

		m_hwndTooltip = CreateToolTipForRectArea(m_hdlgMe,
			_T("This is tooltip for a whole client-area."));

		vaDbgTs(_T("In %s, created tooltip-hwnd=0x%08X."), msz_name, m_hwndTooltip);
		
		*pMsgRet = AcceptDefaultFocus_TRUE;
		return Actioned_yes;
	}
	if (uMsg == WM_NOTIFY)
	{
		dbg_WM_NOTIFY(wParam, lParam);
		return Actioned_no;
	}
	else
		return actioned;
}

#endif // TtDlgForRectArea_IMPL
