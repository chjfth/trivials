#include "shareinc.h"

#include "resource.h"
#include "ModelessTtdemo.h"

class CTtDlgForRectArea : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	virtual void DlgProc(VSeq_t seq,
		UINT uMsg, WPARAM wParam, LPARAM lParam, DlgRet_st *pDlgRet=nullptr) override;
};

#ifdef TtDlgForRectArea_IMPL

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
//	ti.hinst = g_hinstExe; // not required, msdn bug!
	
	ti.lpszText = (PTSTR)pszText; // I believe tooltip internal code will make a string copy.

	// Associate the tooltip with the "tool" window.
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);

	return hwndTT;
}


void 
CTtDlgForRectArea::DlgProc(VSeq_t vseq,
	UINT uMsg, WPARAM wParam, LPARAM lParam, DlgRet_et *pDlgRet)
{
//	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	auto vc = MakeVierachyCall(this, &CModelessTtDemo::DlgProc, vseq,
		uMsg, wParam, lParam, pDlgRet);

	if (uMsg == WM_INITDIALOG)
	{
		// Create the tooltip window.

		m_hwndTooltip = CreateToolTipForRectArea(m_hdlgMe,
			_T("This is tooltip for a whole client-area."));

//		vaDbgTs(_T("Called CreateToolTipForRectArea(), tooltip-hwnd=0x%08X."), m_hwndTooltip);		
		
		pDlgRet->actioned = Actioned_yes;
		pDlgRet->retval = AcceptDefaultFocus_TRUE;
	}
}

#endif // TtDlgForRectArea_IMPL
