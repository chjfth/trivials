#pragma once

#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include "ModelessChild.h"

#include <commdefs.h>

#include <StringHelper.h>
#include <mswin/win32cozy.h> // for RECTtext

#include <mswin/commctrl.itc.h>
using namespace itc;

#include "../utils.h"

extern BOOL g_isTTS_BALLOON;
extern BOOL g_isTTF_CENTERTIP;
extern BOOL g_isWS_EX_TRANSPARENT;

inline DWORD flag_TTS_BALLOON()
{
	return (g_isTTS_BALLOON ? TTS_BALLOON : 0);
}

inline DWORD flag_TTF_CENTERTIP()
{
	return (g_isTTF_CENTERTIP ? TTF_CENTERTIP : 0);
}

inline DWORD flag_WS_EX_TRANSPARENT()
{
	return (g_isWS_EX_TRANSPARENT ? WS_EX_TRANSPARENT : 0);
}


class CModelessTtDemo : public CModelessChild
{
	// This class holds a tooltip HWND that is shared by each concrete
	// tooltip-demo modeless-dialogbox.

protected:

	HWND m_hwndTooltip = NULL;

	enum { BadPos = -32000 };
	RECT m_dlgrect = { BadPos, 0, 0, 0 }; // to remember previous window position

public:

	using CModelessChild::CModelessChild; // this requires C++11, VC2015+

	virtual Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet=nullptr) override
	{
		SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

		Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

		if (uMsg == WM_INITDIALOG)
		{
			DlgTitle_add_BCT();
		}
		if (uMsg == WM_WINDOWPOSCHANGED)
		{
			// WM_WINDOWPOSCHANGED appears *after* CModelessTtDemo's child-class's WM_INITDIALOG,
			// so this executes *after* child-class's enable_Julayout().

			if (m_dlgrect.left != BadPos)
			{
				// restore previous dlg position
				MoveWindow(m_hdlgMe,
					m_dlgrect.left, m_dlgrect.top, RECTwidth(m_dlgrect), RECTheight(m_dlgrect),
					TRUE);

				m_dlgrect.left = BadPos; // must, avoid pinning the window to this pos
			}
		}

		return actioned;
	}

	virtual void DlgClosing() override
	{
		// save current dlg position
		RECT rc = {};
		GetWindowRect(m_hdlgMe, &m_dlgrect);

		if (m_hwndTooltip)
		{
			vaDbgTs(_T("In %s, destroy tooltip-hwnd=0x%08X."), msz_name, m_hwndTooltip);
			DestroyWindow(m_hwndTooltip);
			m_hwndTooltip = NULL;
		}
	}

	void DlgTitle_add_BCT()
	{
		TCHAR szTitle[80] = {};
		GetWindowText(m_hdlgMe, szTitle, ARRAYSIZE(szTitle));

		TCHAR suffix[10] = _T("");
		if (g_isTTS_BALLOON)
			_sntprintf_s(suffix, _TRUNCATE, _T("%sB."), suffix);
		if (g_isTTF_CENTERTIP)
			_sntprintf_s(suffix, _TRUNCATE, _T("%sC."), suffix);
		if (g_isWS_EX_TRANSPARENT)
			_sntprintf_s(suffix, _TRUNCATE, _T("%sT."), suffix);

		int slen = (int)_tcslen(suffix);
		if(slen==0)
		{
			_sntprintf_s(szTitle, _TRUNCATE, _T("%s (-)"), szTitle);
		}
		else
		{
			suffix[slen - 1] = '\0';
			_sntprintf_s(szTitle, _TRUNCATE, _T("%s (%s)"), szTitle, suffix);
		}

		SetWindowText(m_hdlgMe, szTitle);
	}

public:
	template<typename T_TtDemoDlg> // T_TtDemoDlg is CTtDlgForUic etc.
	static BOOL LaunchTootipDemoChildDlg(T_TtDemoDlg &ttdlg,
		const TCHAR *name, UINT iddlg_child, 
		HWND hwndParent, CModelessChild **pptd)
	{
		BOOL succ = 0;
		CModelessChild* &ptd = *pptd;
		if (!ptd)
		{
			CModelessChild *ptdnew = &ttdlg;

			succ = ptdnew->CreateTheDialog(name, g_hinstExe, iddlg_child, hwndParent);
			assert(succ);
			assert(ptdnew == ptd);

			ASSERT_DLG_SUCC(ptd->GetHdlg(), IDD_TooltipForUic);

			return succ ? TRUE : FALSE;
		}
		else
		{
			SetForegroundWindow(ptd->GetHdlg());
			return TRUE;
		}
	}

};


inline void dbg_TTM_ADDTOOL(const TCHAR *textprefix, const TOOLINFO& ti, BOOL succ)
{
	TCHAR rtext[60] = {};
	vaDbgTs(
		_T("%s , TTM_ADDTOOL:\n")
		_T("    ti.uFlags = %s \n")
		_T("    ti.hwnd = 0x%X \n")
		_T("    ti.uId  = 0x%X \n")
		_T("    ti.rect = %s \n")
		_T("    ti.lpszText = %s \n")
		_T("    %s")
		,
		textprefix,
		ITCSv(ti.uFlags, TTF_xxx),
		(UINT_PTR)ti.hwnd,
		(UINT_PTR)ti.uId,
		RECTtext(ti.rect, rtext, ARRAYSIZE(rtext)),
		(ti.lpszText!= LPSTR_TEXTCALLBACK) ? ti.lpszText : _T("-1(callback)"),
		succ ? _T("[Success]") : _T("[Fail]")
		);
}

inline void dbg_WM_NOTIFY(WPARAM wParam, LPARAM lParam)
{
	int uic = (int)wParam;
	NMHDR *pnmh = (NMHDR *)lParam;

	vaDbgTs(_T("WM_NOTIFY from Uic(%d): idFrom=0x%llX, code=%s"),
		uic, (UINT64)pnmh->idFrom, ITCSv(pnmh->code, TTN_xxx));
}



