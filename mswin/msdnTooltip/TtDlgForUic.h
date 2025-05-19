#include "shareinc.h"

#include "resource.h"
#include "ModelessTtdemo.h"

class CTtDlgForUic : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet=nullptr) override;

	virtual void DlgClosing() override;
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
HWND CreateToolTip_ForUic(int toolID, HWND hDlg, PCTSTR pszText,
	bool isTTS_BALLOON)
{
	if (!toolID || !hDlg || !pszText)
	{
		return FALSE;
	}
	// Get the window of the tool.
	HWND hwndTool = GetDlgItem(hDlg, toolID);

	// Create the tooltip. g_hInst is the global instance handle.
	HWND hwndTip = CreateWindowEx(
		NULL, // better to assign WS_EX_TOPMOST here.
		TOOLTIPS_CLASS,
		NULL, // window title
		WS_POPUP | TTS_ALWAYSTIP | (isTTS_BALLOON ? TTS_BALLOON : 0),
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

CModelessChild::Actioned_et
CTtDlgForUic::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
{
	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

	if (uMsg == WM_INITDIALOG)
	{
		// Fetch parameter from UI

		HWND hckb = GetDlgItem(m_hdlgParent, IDCK_TTS_BALLOON);
		int btnCkd = Button_GetCheck(hckb);
		EnableWindow(hckb, FALSE);

		// Create the tooltip window.

		m_hwndTooltip = CreateToolTip_ForUic(IDB_HasTooltip, m_hdlgMe,
			_T("This is the tooltip for the button IDB_HasTootip"),
			btnCkd ? true : false);

		vaDbgTs(_T("In %s, created tooltip-hwnd=0x%08X."), msz_name, m_hwndTooltip);

		SetFocus(GetDlgItem(m_hdlgMe, IDB_HasTooltip));
		*pMsgRet = AcceptDefaultFocus_FALSE;

		return Actioned_yes;
	}
	if (uMsg == WM_COMMAND)
	{
		UINT cmd = GET_WM_COMMAND_ID(wParam, lParam);
		if (cmd == IDB_NoTooltip)
			SetDlgItemText(m_hdlgMe, IDC_EDIT1, _T("IDB_NoTooltip clicked."));
		else if (cmd == IDB_HasTooltip)
			SetDlgItemText(m_hdlgMe, IDC_EDIT1, _T("IDB_HasTooltip clicked."));

		return Actioned_yes;
	}
	else 
		return actioned;
}

void 
CTtDlgForUic::DlgClosing()
{
	__super::DlgClosing();

	HWND hckb = GetDlgItem(m_hdlgParent, IDCK_TTS_BALLOON);
	EnableWindow(hckb, TRUE);
}

#endif // TtDlgForUic_IMPL
