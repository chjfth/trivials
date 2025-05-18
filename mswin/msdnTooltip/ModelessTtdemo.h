#pragma once

#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include "ModelessChild.h"

#include <commdefs.h>

#include <mswin/win32cozy.h> // for RECTtext

#include <mswin/commctrl.itc.h>
using namespace itc;

#include "../utils.h"


class CModelessTtDemo : public CModelessChild
{
	// This class holds a tooltip HWND that is shared by each concrete
	// tooltip-demo modeless-dialogbox.

protected:

	HWND m_hwndTooltip = NULL;

public:

	using CModelessChild::CModelessChild; // this requires C++11, VC2015+

	virtual Actioned_et DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet=nullptr) override
	{
		SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

		Actioned_et actioned = __super::DlgProc(uMsg, wParam, lParam, pMsgRet);

		return actioned;
	}

	virtual void DlgClosing() override
	{
		if (m_hwndTooltip)
		{
			vaDbgTs(_T("Tooltip(hwnd 0x%08X) destroy."), m_hwndTooltip);
			DestroyWindow(m_hwndTooltip);
			m_hwndTooltip = NULL;
		}
	}


public:
	template<typename T_TtDemoDlg> // T_TtDemoDlg is CTtDlgForUic etc.
	static BOOL LaunchTootipDemoChildDlg(const TCHAR *name, UINT iddlg_child, 
		HWND hwndParent, CModelessChild **pptd)
	{
		BOOL succ = 0;
		CModelessChild* &ptd = *pptd;
		if (!ptd)
		{
			CModelessChild *ptdnew = new T_TtDemoDlg(&ptd);

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
		_T("    ti.hwnd = 0x%08X \n")
		_T("    ti.uId  = 0x%08X \n")
		_T("    ti.rect = %s \n")
		_T("    ti.lpszText = %s \n")
		_T("    %s")
		,
		textprefix,
		ITCSv(ti.uFlags, TTF_xxx),
		(UINT_PTR)ti.hwnd,
		(UINT_PTR)ti.uId,
		RECTtext(ti.rect, rtext, ARRAYSIZE(rtext)),
		ti.lpszText,
		succ ? "[Success]" : "[Fail]"
		);
}
