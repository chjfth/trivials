#include "shareinc.h"

#include <mswin/win32cozy.h>

#include "resource.h"
#include "ModelessTtdemo.h"

class CTtDlgMultiline : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet=nullptr) override;

	virtual void DlgClosing() override;

private:
	static const int sar_OptUic[];

	TCHAR *m_pszTooltipText = nullptr;
};


#ifdef TtDlgMultiline_IMPL

HWND CreateToolTip_Multiline(HWND hwndOwner, int uicHottool,
	BOOL isMultiline, int LineWidth)
{
	assert(hwndOwner);
	assert(uicHottool);
	assert(LineWidth >= 0);

	// Get the window of the tool.
	HWND hwndUic = GetDlgItem(hwndOwner, uicHottool);
	assert(hwndUic);

	// Create the tooltip. g_hinstExe is the global instance handle.
	HWND hwndTT = CreateWindowEx(
		WS_EX_TRANSPARENT, // better to add WS_EX_TOPMOST here.
		TOOLTIPS_CLASS,
		NULL, // window title
		WS_POPUP | TTS_ALWAYSTIP,  // need WS_POPUP?
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hwndOwner, // tooltip-window's owner
		NULL,
		g_hinstExe, NULL);

	assert(hwndTT);
	if (!hwndTT)
	{
		vaMsgBox(NULL, MB_OK|MB_ICONERROR, NULL, _T("Tooltip-window creation error."));
		return NULL;
	}

	// Associate the tooltip with the tool.
	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.hwnd = hwndOwner;
	ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
	ti.uId = (UINT_PTR)hwndUic;
	ti.lpszText = LPSTR_TEXTCALLBACK;
	
	// Associate the tooltip with the "tool" window.
	LRESULT succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);

	if (isMultiline)
	{
		SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, LineWidth);
	}
	else
	{
		SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, (-1));
	}


	dbg_TTM_ADDTOOL(_T("CreateToolTip_Multiline()"), ti, (BOOL)succ);

	return hwndTT;
}

const int CTtDlgMultiline::sar_OptUic[] = {
	IDCK_EnableMultiline, IDE_LineWidth};


CModelessChild::Actioned_et
CTtDlgMultiline::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
{
	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

	if (uMsg == WM_INITDIALOG)
	{
		// Fetch parameter from UI

		BOOL isMultiline = IsDlgButtonChecked(m_hdlgParent, IDCK_EnableMultiline);
		BOOL bTrans = 0;
		int LineWidth = GetDlgItemInt(m_hdlgParent, IDE_LineWidth, &bTrans, bSigned_FALSE);

		Enable_Uics(FALSE, m_hdlgParent, sar_OptUic);

		// Create the tooltip window.

		m_hwndTooltip = CreateToolTip_Multiline(m_hdlgMe, IDB_HasTooltip,
			isMultiline, LineWidth);

		vaDbgTs(_T("In %s, created tooltip-hwnd=0x%08X."), msz_name, m_hwndTooltip);

		SetFocus(GetDlgItem(m_hdlgMe, IDB_HasTooltip));
		*pMsgRet = AcceptDefaultFocus_FALSE;

		return Actioned_yes;
	}
	if (uMsg == WM_NOTIFY)
	{
		dbg_WM_NOTIFY(wParam, lParam);

		int uic = (int)wParam;
		NMHDR *pnmh = (NMHDR *)lParam;
		HWND hwndFrom = pnmh->hwndFrom;
		
		if (hwndFrom == m_hwndTooltip)
		{
			if(pnmh->code == (TTN_NEEDTEXT|TTN_GETDISPINFO)) // same value
			{
				delete m_pszTooltipText; // delete old text buffer

				HWND hwndDyntext = GetDlgItem(m_hdlgParent, IDE_MultilineText);
				int slen = SendMessage(hwndDyntext, WM_GETTEXTLENGTH, 0, 0);
				if (slen <= 0)
					return Actioned_no;

				m_pszTooltipText = new TCHAR[slen + 1];
				GetDlgItemText(m_hdlgParent, IDE_MultilineText, m_pszTooltipText, slen+1);

				NMTTDISPINFO *pdi = (NMTTDISPINFO *)pnmh;
				pdi->lpszText = m_pszTooltipText;

				*pMsgRet = TRUE;
				return Actioned_yes;
			}
		}

		return Actioned_no;
	}
	if (uMsg == WM_COMMAND)
	{
		UINT uic = GET_WM_COMMAND_ID(wParam, lParam);
		if (uic == IDB_NoTooltip)
			SetDlgItemText(m_hdlgMe, IDC_EDIT1, _T("IDB_NoTooltip clicked."));
		else if (uic == IDB_HasTooltip)
			SetDlgItemText(m_hdlgMe, IDC_EDIT1, _T("IDB_HasTooltip clicked."));

		return Actioned_yes;
	}
	else 
		return actioned;
}

void 
CTtDlgMultiline::DlgClosing()
{
	__super::DlgClosing();

	delete m_pszTooltipText;

	Enable_Uics(TRUE, m_hdlgParent, sar_OptUic);
}

#endif // TtDlgMultiline_IMPL
