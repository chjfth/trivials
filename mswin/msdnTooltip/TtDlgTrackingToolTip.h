#include "shareinc.h"

#include "resource.h"
#include "ModelessTtdemo.h"

#include <mswin/WM_MOUSELEAVE_helper.h>
#include <mswin/win32cozy.h>


class CTtDlgTrackingTooltip_concise : public CModelessTtDemo
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


class CTtDlgTrackingTooltip_misc : public CModelessTtDemo
{
public:
	using CModelessTtDemo::CModelessTtDemo; // this requires C++11, VC2015+

	Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet=nullptr) override;

	virtual void DlgClosing() override;

private:
	CWmMouseleaveHelper m_mouseleave;

	TOOLINFO m_ti = { sizeof(TOOLINFO) };
	int m_oldX = 0, m_oldY = 0;

	int m_offsetX, m_offsetY; // tooltip's offset to mouse-position 

	BOOL m_isClientToScreen = TRUE;

private:
	static const int sar_OptUic[];
};


//////////////////////////////////////////////////////////////////////////
// Implementation below
//////////////////////////////////////////////////////////////////////////

#ifdef TtDlgTrackingToolTip_IMPL


HWND CreateTrackingTooltip_FreeOnScreen(HWND hwndOwner=nullptr)
{
	// FreeOnScreen implies: TOOLINFO.hwnd and TOOLINFO.uId are both NULL;

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
	LRESULT succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);

	dbg_TTM_ADDTOOL(_T("CreateTrackingTooltip_FreeOnScreen()"), ti, (BOOL)succ);

	return hwndTT;
}


CModelessChild::Actioned_et
CTtDlgTrackingTooltip_concise::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
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
			_T("Tooltip showing mouse-pos throughout whole dialog-box client-area.\r\n")
			_T("Tooltip offset (%d, %d) from actual mouse-pos.")
			,
			m_offsetX, m_offsetY);

		return Actioned_yes;
	}
	if (uMsg == WM_MOUSEMOVE)
	{
		CWmMouseleaveHelper::Move_ret moveret = m_mouseleave.do_WM_MOUSEMOVE();

		// Activate(make visible) the tooltip.
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
			WCHAR coords[40];
			_sntprintf_s(coords, _TRUNCATE, _T("%d, %d"), newX, newY);

			// Set new text of the tooltip.
			ti.lpszText = coords;
			SendMessage(m_hwndTooltip, TTM_UPDATETIPTEXT, 0, (LPARAM)&ti);
			// -- Note: Don't merely use TTM_SETTOOLINFO here, which has ti.uFlags==0,
			// (no TTF_TRACK flag bit), which in turn causes TTM_TRACKPOSITION's x,y coords
			// to become useless.
			// If we use TTM_UPDATETIPTEXT, then the tooltip code does not care ti.uFlags,
			// so the initial TTF_TRACK(when we did TTM_ADDTOOL) feature is preserved.

			// Position the tooltip. The coordinates are adjusted so that 
			// the tooltip does not overlap the mouse pointer.
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
	if (uMsg == WM_NOTIFY)
	{
		// will not see this bcz the tooltip does not spin up spontaneously
		dbg_WM_NOTIFY(wParam, lParam);
		return Actioned_no;
	}
	else
		return actioned;
}

void CTtDlgTrackingTooltip_concise::DlgClosing()
{
	__super::DlgClosing();

	EnableDlgItem(m_hdlgParent, IDE_TtOffsetX);
	EnableDlgItem(m_hdlgParent, IDE_TtOffsetY);
}


HWND CreateTrackingToolTip_misc(HWND hwndOwner, TOOLINFO& ti,
	BOOL isTTF_TRACK, BOOL isTTF_ABSOLUTE)
{
	// Create a tooltip.

	HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, 
		TOOLTIPS_CLASS, 
		NULL, // window title
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hwndOwner, 
		NULL, g_hinstExe, NULL);

	if (!hwndTT)
		return NULL;

	ti.cbSize = sizeof(TOOLINFO);
	
	ti.uFlags = TTF_IDISHWND;
	if (isTTF_TRACK)
		ti.uFlags |= TTF_TRACK;
	if (isTTF_ABSOLUTE)
		ti.uFlags |= TTF_ABSOLUTE;
	
	ti.hwnd = NULL;
	ti.uId = (UINT_PTR)hwndOwner; // the dlgbox handle
	ti.lpszText = _T("Initial tooltip text"); // this text will be overwritten soon
	SetRect(&ti.rect, 0, 0, 0, 0);

	// Associate the tooltip with the tool window.
	LRESULT succ = SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)&ti);

	dbg_TTM_ADDTOOL(_T("CreateTrackingTooltip_FreeOnScreen()"), ti, (BOOL)succ);

	return hwndTT;
}

const int CTtDlgTrackingTooltip_misc::sar_OptUic[] = {
	IDCK_TTF_TRACK, IDCK_TTF_ABSOLUTE, IDCK_ClientToScreen };

CModelessChild::Actioned_et
CTtDlgTrackingTooltip_misc::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
{
	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

	if (uMsg == WM_INITDIALOG)
	{
		BOOL isTTF_TRACK = IsDlgButtonChecked(m_hdlgParent, IDCK_TTF_TRACK);
		BOOL isTTF_ABSOLUTE = IsDlgButtonChecked(m_hdlgParent, IDCK_TTF_ABSOLUTE);
		m_isClientToScreen = IsDlgButtonChecked(m_hdlgParent, IDCK_ClientToScreen);

		BOOL bTrans = 0;
		m_offsetX = GetDlgItemInt(m_hdlgParent, IDE_TtOffsetX, &bTrans, bSigned_TRUE);
		m_offsetY = GetDlgItemInt(m_hdlgParent, IDE_TtOffsetY, &bTrans, bSigned_TRUE);

		// Create the tooltip window.

		m_hwndTooltip = CreateTrackingToolTip_misc(m_hdlgMe, m_ti, isTTF_TRACK, isTTF_ABSOLUTE);
		assert(m_hwndTooltip);

		m_mouseleave.SetHwnd(m_hdlgMe);

		vaDbgTs(_T("In %s, created tooltip-hwnd=0x%08X."), msz_name, m_hwndTooltip);

		SetWindowText(m_hdlgMe, _T("Tooltip tracking misc test"));
		vaSetDlgItemText(m_hdlgMe, IDC_STATIC1,
			_T("TTF_TRACK = %s\r\n")
			_T("TTM_TRACKPOSITION ClientToScreen = %s\r\n")
			_T("TTF_ABSOLUTE = %s\r\n")
			_T("offsetX: %d , offsetY: %d")
			,
			isTTF_TRACK ? _T("yes") : _T("no"),
			m_isClientToScreen ? _T("yes") : _T("no"),
			isTTF_ABSOLUTE ? _T("yes") : _T("no"),
			m_offsetX, m_offsetY);
		
		Enable_Uics(FALSE, m_hdlgParent, sar_OptUic);

		return Actioned_yes;
	}
	if (uMsg == WM_MOUSEMOVE)
	{
		CWmMouseleaveHelper::Move_ret moveret = m_mouseleave.do_WM_MOUSEMOVE();

		// Activate(make visible) the tooltip.
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
			WCHAR coords[40];
			_sntprintf_s(coords, _TRUNCATE, _T("mouse: %d, %d"), newX, newY);

			m_ti.lpszText = coords;
			SendMessage(m_hwndTooltip, TTM_SETTOOLINFO, 0, (LPARAM)&m_ti);

			// Position the tooltip. The coordinates are adjusted so that the tooltip does not overlap the mouse pointer.

			POINT pt_ttm = { newX + m_offsetX, newY + m_offsetY };

			if(m_isClientToScreen)
				ClientToScreen(m_hdlgMe, &pt_ttm);

			vaDbgTs(_T("Mouse at client [%d,%d]; pt_ttm [%d,%d]"),
				newX, newY,  pt_ttm.x, pt_ttm.y);

			vaSetDlgItemText(m_hdlgMe, IDC_STATIC2,
				_T("TTM_TRACKPOSITION use [%d, %d]"), pt_ttm.x, pt_ttm.y);

			SendMessage(m_hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(pt_ttm.x, pt_ttm.y));
	}

		return Actioned_yes;
	}
	else if (uMsg == WM_MOUSELEAVE)
	{
		m_mouseleave.do_WM_MOUSELEAVE();

		vaSetDlgItemText(m_hdlgMe, IDC_STATIC2, _T("Mouse outside the dialog-box area."));

		// The mouse pointer has left our window. Deactivate the tooltip.
		SendMessage(m_hwndTooltip, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&m_ti);

		return Actioned_yes;
	}
	if (uMsg == WM_NOTIFY)
	{
		// will not see this bcz the tooltip does not spin up spontaneously
		dbg_WM_NOTIFY(wParam, lParam);
		return Actioned_no;
	}
	else
		return actioned;
}

void CTtDlgTrackingTooltip_misc::DlgClosing()
{
	__super::DlgClosing();

	Enable_Uics(TRUE, m_hdlgParent, sar_OptUic);
}


#endif // TtDlgTrackingToolTip_IMPL
