#include "shareinc.h"

#include "resource.h"
#include "ModelessTtdemo.h"

class CTtDlgForUic : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	Relay_et DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#ifdef TtDlgForUic_IMPL

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
		WS_POPUP | TTS_ALWAYSTIP | TTS_BALLOON,
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

CModelessChild::Relay_et
CTtDlgForUic::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CModelessTtDemo::DlgProc(uMsg, wParam, lParam);

	if (uMsg == WM_INITDIALOG)
	{
		// Create the tooltip window.

		m_hwndTooltip = CreateToolTip_ForUic(IDB_HasTooltip, m_hdlgMe,
			_T("This is the tooltip for the button IDB_HasTootip"));

		vaDbgTs(_T("Called CreateToolTip_ForUic(), tooltip-hwnd=0x%08X."), m_hwndTooltip);
	}

	return Relay_no;
}

#endif // TtDlgForUic_IMPL
