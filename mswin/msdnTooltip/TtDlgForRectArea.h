#include "shareinc.h"

#include "resource.h"
#include "ModelessTtdemo.h"

class CTtDlgForRectArea : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	DoneSth_et DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
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
	ti.hinst = g_hinstExe; // required?
	ti.lpszText = (PTSTR)pszText; // I believe tooltip internal code will make a string copy.

								  // Associate the tooltip with the "tool" window.
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);

	return hwndTT;
}


CModelessChild::DoneSth_et
CTtDlgForRectArea::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CModelessTtDemo::DlgProc(uMsg, wParam, lParam);

	if (uMsg == WM_INITDIALOG)
	{
		// Create the tooltip window.

		m_hwndTooltip = CreateToolTipForRectArea(m_hdlgMe,
			_T("This is tooltip for a whole client-area."));

		vaDbgTs(_T("Called CreateToolTipForRectArea(), tooltip-hwnd=0x%08X."), m_hwndTooltip);
	}

	return DoneSth_yes;
}

#endif // TtDlgForRectArea_IMPL
