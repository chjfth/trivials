#pragma once

#include <assert.h>
#include <windows.h>
#include <windowsx.h>
#include "ModelessChild.h"

#include <commdefs.h>
#include "../utils.h"

#include <CxxVerCheck.h>
#ifndef CXX11_OR_NEWER
#error "This header file requires "
#endif


class CModelessTtDemo : public CModelessChild
{
	// This class holds a tooltip HWND that is shared by each concrete
	// tooltip-demo modeless-dialogbox.

protected:

	HWND m_hwndTooltip = NULL;

public:

	using CModelessChild::CModelessChild; // this requires C++11, VC2015+

	virtual DLGPROC_ret DlgProc(
		UINT uMsg, WPARAM wParam, LPARAM lParam, DoneSth_et *pDoneSth=nullptr) override
	{
		SETTLE_OUTPUT_PTR(DoneSth_et, pDoneSth, DoneSth_no);

		DLGPROC_ret dlgret = CModelessChild::DlgProc(uMsg, wParam, lParam, pDoneSth);

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
			
			*pDoneSth = DoneSth_yes;
			return 0;
		}
		else
			return dlgret;
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