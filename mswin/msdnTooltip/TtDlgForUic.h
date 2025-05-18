#include "shareinc.h"

#include "resource.h"
#include "ModelessTtdemo.h"

class CTtDlgForUic : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	virtual void DlgProc(VSeq_t vseq,
		UINT uMsg, WPARAM wParam, LPARAM lParam, DlgRet_st *pDlgRet=nullptr) override;

	virtual void DlgClosing(VSeq_t vseq) override;
};


//////////////////////////////////////////////////////////////////////////
// Implementation code below
//////////////////////////////////////////////////////////////////////////

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

void 
CTtDlgForUic::DlgProc(VSeq_t vseq,
	UINT uMsg, WPARAM wParam, LPARAM lParam, DlgRet_st *pDlgRet)
{
//	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	auto vc = MakeVierachyCall(this, &CModelessTtDemo::DlgProc, vseq,
		uMsg, wParam, lParam, pDlgRet);

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

		vaDbgTs(_T("Called CreateToolTip_ForUic(), tooltip-hwnd=0x%08X."), m_hwndTooltip);

		SetFocus(GetDlgItem(m_hdlgMe, IDB_HasTooltip));
		pDlgRet->actioned = Actioned_yes;
		pDlgRet->retval = AcceptDefaultFocus_FALSE;

	}
	if (uMsg == WM_COMMAND)
	{
		UINT cmd = GET_WM_COMMAND_ID(wParam, lParam);
		if (cmd == IDB_NoTooltip)
			SetDlgItemText(m_hdlgMe, IDC_EDIT1, _T("IDB_NoTooltip clicked."));
		else if (cmd == IDB_HasTooltip)
			SetDlgItemText(m_hdlgMe, IDC_EDIT1, _T("IDB_HasTooltip clicked."));

//		return Actioned_yes;
	}
}

void 
CTtDlgForUic::DlgClosing(VSeq_t vseq)
{
	auto vc = MakeVierachyCall(this, &CModelessTtDemo::DlgClosing, vseq);

	HWND hckb = GetDlgItem(m_hdlgParent, IDCK_TTS_BALLOON);
	EnableWindow(hckb, TRUE);
}

#endif // TtDlgForUic_IMPL
