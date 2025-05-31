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
//   Creates a tooltip for a control(Uic) in a dialog box.
// Parameters:
//   hDlg - window handle of the dialog box.
//   idTool - Control-ID in hDlg dialog box.
//   pszText - string to use as the tooltip text.
// Returns:
//   The HWND of the newly created the tooltip.
//
HWND CreateToolTip_ForUic(HWND hDlg, int toolID, PCTSTR pszText)
{
	HWND hwndOwner = hDlg;

	if (!toolID || !hDlg || !pszText)
	{
		return FALSE;
	}

	// Get the HWND of the tool.
	HWND hwndTool = GetDlgItem(hDlg, toolID);

	// Create the tooltip. g_hinstExe is the global instance handle.
	HWND hwndTT = CreateWindowEx(
		WS_EX_TOPMOST | flag_WS_EX_TRANSPARENT(),
		TOOLTIPS_CLASS,
		NULL, // window title
		TTS_ALWAYSTIP | flag_TTS_BALLOON(),
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hwndOwner, // tooltip-window's owner, receiving WM_NOTIFY
		NULL,
		g_hinstExe, NULL);

	if (!hwndTool || !hwndTT)
	{
		return NULL;
	}

	// Associate the tooltip with the tool.
	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.hwnd = hDlg;
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS | flag_TTF_CENTERTIP();
	ti.uId = (UINT_PTR)hwndTool;
	ti.lpszText = (PTSTR)pszText; // I believe tooltip internal code will make a string copy.
	
	// Associate the tooltip with the "tool" window.
	LRESULT succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);

	dbg_TTM_ADDTOOL(_T("CreateToolTip_ForUic()"), ti, (BOOL)succ);

	return hwndTT;
}

CModelessChild::Actioned_et
CTtDlgForUic::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
{
	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

	if (uMsg == WM_INITDIALOG)
	{
		// Create the tooltip window.

		m_hwndTooltip = CreateToolTip_ForUic(m_hdlgMe, IDB_BtnHasTooltip,
			_T("This is the tooltip for the button IDB_BtnHasTooltip"));

		vaDbgTs(_T("In %s, created tooltip-hwnd=0x%08X."), msz_name, m_hwndTooltip);

		SetFocus(GetDlgItem(m_hdlgMe, IDB_BtnHasTooltip));
		*pMsgRet = AcceptDefaultFocus_FALSE;

		return Actioned_yes;
	}
	if (uMsg == WM_NOTIFY)
	{
		dbg_WM_NOTIFY(wParam, lParam);
		return Actioned_no;
	}
	if (uMsg == WM_COMMAND)
	{
		UINT uic = GET_WM_COMMAND_ID(wParam, lParam);
		if (uic == IDB_BtnNoTooltip)
			SetDlgItemText(m_hdlgMe, IDC_EDIT1, _T("IDB_BtnNoTooltip clicked."));
		else if (uic == IDB_BtnHasTooltip)
			SetDlgItemText(m_hdlgMe, IDC_EDIT1, _T("IDB_BtnHasTooltip clicked."));

		return Actioned_yes;
	}
	else 
		return actioned;
}

void 
CTtDlgForUic::DlgClosing()
{
	__super::DlgClosing();
}

#endif // TtDlgForUic_IMPL
