#include "shareinc.h"

#include "resource.h"
#include "ModelessTtdemo.h"

#include <mswin/WM_MOUSELEAVE_helper.h>



class CTtDlgTrackingToolTip : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet=nullptr) override;

private:
	CWmMouseleaveHelper m_mouseleave;

	TOOLINFO m_ti = { sizeof(TOOLINFO) };
	int m_oldX = 0, m_oldY = 0;
};

#ifdef TtDlgTrackingToolTip_IMPL

HWND CreateTrackingToolTip(HWND hwndOwner, const TCHAR* pText, TOOLINFO& ti)
{
	// Create a tooltip.
	HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hwndOwner, NULL, g_hinstExe, NULL);

	if (!hwndTT)
	{
		return NULL;
	}

	// Set up the tool information. In this case, the "tool" is the entire parent window.

	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND 
		| TTF_TRACK   // key point
		| TTF_ABSOLUTE;
//	ti.hwnd = hDlg; // [2017-10-09] tried, can be NULL
//	ti.hinst = g_hInst;
	ti.lpszText = (TCHAR *)pText;
//	ti.uId = (UINT_PTR)hDlg; // [2017-10-09] tried, can be NULL

//	GetClientRect(hDlg, &g_toolItem.rect);

	// Associate the tooltip with the tool window.
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);

	return hwndTT;
}


CModelessChild::Actioned_et
CTtDlgTrackingToolTip::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
{
	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

	if (uMsg == WM_INITDIALOG)
	{
		// Create the tooltip window.

		m_hwndTooltip = CreateTrackingToolTip(m_hdlgMe,
			_T("This is a tracking tooltip"),
			m_ti);

		m_mouseleave.SetHwnd(m_hdlgMe);

		vaDbgTs(_T("Called CreateTrackingToolTip(), tooltip-hwnd=0x%08X."), m_hwndTooltip);

		return Actioned_yes;
	}
	if (uMsg == WM_MOUSEMOVE)
	{
		CWmMouseleaveHelper::Move_ret moveret = m_mouseleave.do_WM_MOUSEMOVE();

		// Activate the tooltip.
		SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&m_ti);

		int newX = GET_X_LPARAM(lParam);
		int newY = GET_Y_LPARAM(lParam);

		// Make sure the mouse has actually moved. The presence of the tooltip 
		// causes Windows to send the message continuously.

		if ((newX != m_oldX) || (newY != m_oldY))
		{
			m_oldX = newX;
			m_oldY = newY;

			// Update the text.
			WCHAR coords[20];
			_sntprintf_s(coords, _TRUNCATE, _T("%d, %d"), newX, newY);

			m_ti.lpszText = coords;
			SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ti);

			// Position the tooltip. The coordinates are adjusted so that the tooltip does not overlap the mouse pointer.

			POINT pt = { newX, newY };
			ClientToScreen(m_hdlgMe, &pt);
			SendMessage(m_hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt.x + 10, pt.y - 20));
	}

		return Actioned_yes;
	}
	else if (uMsg == WM_MOUSELEAVE)
	{
		m_mouseleave.do_WM_MOUSELEAVE();

		// The mouse pointer has left our window. Deactivate the tooltip.
		SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&m_ti);

		return Actioned_yes;
	}
	else 
		return actioned;
}

#endif // TtDlgTrackingToolTip_IMPL
