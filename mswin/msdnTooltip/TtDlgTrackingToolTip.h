#include "shareinc.h"

#include "resource.h"
#include "ModelessTtdemo.h"

#include <mswin/WM_MOUSELEAVE_helper.h>


class CTtDlgTrackingTooltip_LiveMousePos : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet = nullptr) override;

	virtual void DlgClosing() override;

private:
	CWmMouseleaveHelper m_mouseleave;
	int m_oldX = 0, m_oldY = 0;

	int m_offsetX, m_offsetY; // tooltip's offset to mouse-position 
};


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


//////////////////////////////////////////////////////////////////////////
// Implementation below
//////////////////////////////////////////////////////////////////////////

#ifdef TtDlgTrackingToolTip_IMPL


HWND CreateTrackingTooltip_FreeOnScreen(HWND hwndOwner=nullptr)
{
	// Create the tooltip-window.
	HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, 
		TOOLTIPS_CLASS, 
		NULL, // window title
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hwndOwner, 
		NULL, g_hinstExe, NULL);

	if (!hwndTT)
		return NULL;

	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
	// -- TTF_TRACK: User can use TTM_TRACKPOSITION to set its position.
	// -- TTF_ABSOLUTE: Makes TTM_TRACKPOSITION(X,Y) genuine, no encircling effect.
	ti.hwnd = NULL;
	ti.uId = NULL; // We want "whole screen", so these two are NULL.

	// Associate the tooltip with the tool window.
	BOOL succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);

	dbg_TTM_ADDTOOL(_T("CreateTrackingTooltip_FreeOnScreen()"), ti, succ);

	return hwndTT;
}


CModelessChild::Actioned_et
CTtDlgTrackingTooltip_LiveMousePos::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
{
	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

	if (uMsg == WM_INITDIALOG)
	{
		// Create the tooltip window.

		m_hwndTooltip = CreateTrackingTooltip_FreeOnScreen(m_hdlgMe);
		assert(m_hwndTooltip);

		m_mouseleave.SetHwnd(m_hdlgMe);

		vaDbgTs(_T("In %s, created tooltip-hwnd=0x%08X."), msz_name, m_hwndTooltip);

		// Fetch user params from dlgbox UI.
		BOOL bTrans = 0;
		m_offsetX = GetDlgItemInt(m_hdlgParent, IDE_TtOffsetX, &bTrans, bSigned_TRUE);
		m_offsetY = GetDlgItemInt(m_hdlgParent, IDE_TtOffsetY, &bTrans, bSigned_TRUE);

		DisableDlgItem(m_hdlgParent, IDE_TtOffsetX);
		DisableDlgItem(m_hdlgParent, IDE_TtOffsetY);

		SetWindowText(m_hdlgMe, _T("Tooltip tracking concise way"));

		vaSetDlgItemText(m_hdlgMe, IDC_STATIC1,
			_T("Tooltip showing mouse-pos throughout whole dialog-box client-area. \r\n")
			_T("Tooltip offset (%d, %d) from actual mouse-pos.")
			,
			m_offsetX, m_offsetY);

		return Actioned_yes;
	}
	if (uMsg == WM_MOUSEMOVE)
	{
		CWmMouseleaveHelper::Move_ret moveret = m_mouseleave.do_WM_MOUSEMOVE();

		// Activate the tooltip (make tooltip-window appear).
		TOOLINFO ti = { sizeof(TOOLINFO) };
		SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&ti);

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

			// Now we'll use TTM_SETTOOLINFO to set new tooltip-text.
			// Before doing this, we need to call TTM_GETTOOLINFO to fetch the tooltip's
			// original TOOLINFO.uFlags value. By this way, we do not need a global TOOLINFO var.
			// A zero .uFlags would totally change the tooltip's behavior.

			SendMessage(m_hwndTooltip, TTM_GETTOOLINFO, 0, (LPARAM)&ti);
			assert(ti.uFlags != 0);

			// Set new text of the tooltip.
			ti.lpszText = coords;
			SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&ti);

			// Position the tooltip. The coordinates are adjusted so that the tooltip does not overlap the mouse pointer.

			POINT pt_ttm = { newX + m_offsetX, newY + m_offsetY };
			ClientToScreen(m_hdlgMe, &pt_ttm);

			vaDbgTs(_T("Mouse at client [%d,%d]; pt_ttm [%d,%d]"),
				newX, newY, pt_ttm.x, pt_ttm.y);

			SendMessage(m_hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt_ttm.x, pt_ttm.y));
		}

		return Actioned_yes;
	}
	else if (uMsg == WM_MOUSELEAVE)
	{
		m_mouseleave.do_WM_MOUSELEAVE();

		TOOLINFO ti = { sizeof(TOOLINFO) }; // meaningful: .hwnd=NULL and .uId=NULL

		// The mouse pointer has left our window. Deactivate(hide) the tooltip.
		SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&ti);

		return Actioned_yes;
	}
	else
		return actioned;
}

void CTtDlgTrackingTooltip_LiveMousePos::DlgClosing()
{
	__super::DlgClosing();

	EnableDlgItem(m_hdlgParent, IDE_TtOffsetX);
	EnableDlgItem(m_hdlgParent, IDE_TtOffsetY);
}




HWND CreateTrackingToolTip_raw(HWND hwndOwner)
{
	return 0;
}

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

	TCHAR rtext[60] = {};

	// Set up the tool information. In this case, the "tool" is the entire parent window.

	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_IDISHWND
		;// | TTF_TRACK   // key point
		; // | TTF_ABSOLUTE;
	ti.hwnd = hwndOwner; // [2017-10-09] tried, can be NULL
//	ti.hinst = g_hInst;
	ti.lpszText = (TCHAR *)pText;

	HWND hstatic = GetDlgItem(hwndOwner, IDC_STATIC1);
	ti.uId = (UINT_PTR)hstatic; // hwndOwner; // [2017-10-09] tried, can be NULL

	GetClientRect(hstatic, &ti.rect);
//SetRect(&ti.rect, 20,20, 120,120);

	vaDbgTs(
		_T("CreateTrackingToolTip(), TTM_ADDTOOL:\n")
		_T("    ti.uFlags = %s \n")
		_T("    ti.hwnd = 0x%08X \n")
		_T("    ti.uId  = 0x%08X \n")
		_T("    ti.rect = %s \n")
		_T("    ti.lpszText = %s")
		,
		ITCSv(ti.uFlags, TTF_xxx),
		(UINT_PTR)ti.hwnd,
		(UINT_PTR)ti.uId,
		RECTtext(ti.rect, rtext, ARRAYSIZE(rtext)),
		ti.lpszText);

	// Associate the tooltip with the tool window.
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);


	// Add 2nd hottool
	HWND hstatic2 = GetDlgItem(hwndOwner, IDC_STATIC2);
	ti.uId = (UINT_PTR)hstatic2;
	ti.lpszText = _T("Second hottool.");
	BOOL succ2 = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);


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


#if 1
TOOLINFO lti = { sizeof(lti) };
//SetRect(&lti.rect, 100, 100, 200, 200);
lti.uId = (UINT_PTR)GetDlgItem(m_hdlgMe, IDC_STATIC2);
lti.hwnd = m_hdlgMe; // GetDlgItem(m_hdlgMe, IDC_STATIC1);

#else
TOOLINFO lti = m_ti; lti.uFlags = 0;
//lti.hwnd = 0; // would ruin, .hwnd is must
//lti.uId = (UINT_PTR)m_hdlgMe;
//lti.uId = (UINT_PTR)GetDlgItem(m_hdlgMe, IDC_STATIC_TEXT);
//lti.hwnd = GetDlgItem(m_hdlgMe, IDC_STATIC_TEXT);
SetRect(&lti.rect, 0, 0, 0, 0); // OK. lti.rect is not a must
lti.lpszText = NULL; // ok. not must
#endif // 0

		// Activate the tooltip.
		SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&lti);

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
			_sntprintf_s(coords, _TRUNCATE, _T("%d, %d ww"), newX, newY);

			m_ti.lpszText = coords;
//			SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ti);

			// Position the tooltip. The coordinates are adjusted so that the tooltip does not overlap the mouse pointer.

			POINT pt_ttm = { newX+10, newY+10 };
//			ClientToScreen(m_hdlgMe, &pt_ttm);

			vaDbgTs(_T("Mouse at client [%d,%d]; pt_ttm [%d,%d]"),
				newX, newY,  pt_ttm.x, pt_ttm.y);

			SendMessage(m_hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt_ttm.x, pt_ttm.y));
	}

		return Actioned_yes;
	}
	else if (uMsg == WM_MOUSELEAVE)
	{
		m_mouseleave.do_WM_MOUSELEAVE();

		// The mouse pointer has left our window. Deactivate the tooltip.
//		SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&m_ti);

		return Actioned_yes;
	}
	else 
		return actioned;
}

#endif // TtDlgTrackingToolTip_IMPL
