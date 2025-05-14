#pragma once

#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include "ModelessChild.h"

#include <commdefs.h>
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

		if (uMsg == WM_COMMAND)
		{
			assert(mr_ptrme_by_parent == this);

			UINT cmd = GET_WM_COMMAND_ID(wParam, lParam);
			if (cmd == IDCANCEL)
			{
				if (m_hwndTooltip)
				{
					vaDbgTs(_T("Tooltip(hwnd 0x%08X) destroy."), m_hwndTooltip);
					DestroyWindow(m_hwndTooltip);
					m_hwndTooltip = NULL;
				}
			}
			
			return Actioned_yes;
		}
		else
			return actioned;
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