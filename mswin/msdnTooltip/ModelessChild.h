#ifndef __ModelessChild_h_20250713_
#define __ModelessChild_h_20250713_

// This .h can be a common library, not just used in a single EXE project.

#include <tchar.h>
#include <windows.h>

#include <commdefs.h>
#include <mswin/win32clarify.h>
#include <SdringTCHAR.h>

#include <CxxVerCheck.h>
#ifndef CXX11_OR_NEWER
#error "This header file requires VC2015+ compiler."
#endif


class CModelessChild
{
	// This class helps the parent-dialog manage this object as modeless child-dialog.

public:
	CModelessChild(CModelessChild** pptrme_by_parent) : mr_ptrme_by_parent(*pptrme_by_parent)
	{}

	BOOL CreateTheDialog(const TCHAR *name,
		HINSTANCE hInstExe, int dlg_resid, HWND hdlgParent, LPCDLGTEMPLATE pDlgTemplate=NULL);

	HWND GetHdlg() { return m_hdlgMe; } // debug purpose

protected:
	enum Actioned_et { Actioned_no = 0 , Actioned_yes = 1 };
	
	virtual Actioned_et DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam,
		INT_PTR *pMsgRet=nullptr);
	// -- Note: The function body reports dlgbox-message processing result
	// by returning Actioned_et and setting *pMsgRet. 
	// The function body do not need to call SetDlgMsgResult(), bcz it will be called 
	// by StartDlgProc() for you.

	static DLGPROC_ret WINAPI StartDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual void DlgClosing() {};

protected:
	Sdring msd_name;
	const TCHAR *msz_name = nullptr;
	HWND m_hdlgMe = nullptr;
	HWND m_hdlgParent = nullptr;

	CModelessChild* &mr_ptrme_by_parent; // key member
	UINT m_WM_KillSelf = 0;
};


///////////////////////////////////////////////////////////////
// Implementation Below:
///////////////////////////////////////////////////////////////

#ifdef ModelessChild_IMPL

#include <assert.h>
#include <WindowsX.h>

#ifndef ModelessChild_DEBUG
#include <CHHI_vaDBG_hide.h>
#else
#include <mswin/winuser.itc.h>
#endif


BOOL CModelessChild::CreateTheDialog(const TCHAR *name, 
	HINSTANCE hInstExe, int dlg_resid, HWND hdlgParent, LPCDLGTEMPLATE pDlgTemplate)
{
	// Note: When identifying a dlgbox, user choose between dlg_resid or pDlgTemplate.
	// If dlg_resid==0, pDlgTemplate is used.

	msd_name = name;
	msz_name = msd_name;

	m_hdlgParent = hdlgParent;

	mr_ptrme_by_parent = this;
	// -- mr_ptrme_by_parent should better be set before calling CreateDialogParam(),
	// bcz WM_INITDIALOG's calling SetDlgItemText(m_hdlgMe, IDC_SOME_EDITBOX, ...)
	// could trigger m_hdlgMe's WM_COMMAND(EN_UPDATE), where we like to see 
	// mr_ptrme_by_parent is non-NULL. CTtDlgInplaceSimplest can trigger this.

	if(dlg_resid)
	{
		m_hdlgMe = CreateDialogParam(hInstExe, MAKEINTRESOURCE(dlg_resid), hdlgParent,
			StartDlgProc,
			(LPARAM)this // dlgparam
		);
	}
	else
	{
		m_hdlgMe = CreateDialogIndirectParam(hInstExe, pDlgTemplate, hdlgParent,
			StartDlgProc,
			(LPARAM)this // dlgparam
		);
	}

	m_WM_KillSelf = RegisterWindowMessage(_T("ModelessChild_KillSelf"));

	return m_hdlgMe ? TRUE : FALSE;
}

DLGPROC_ret WINAPI 
CModelessChild::StartDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CModelessChild *pMydlg = (CModelessChild*)GetWindowLongPtr(hdlg, DWLP_USER);

	if (uMsg == WM_INITDIALOG)
	{
		assert(!pMydlg);
		assert(lParam);

		pMydlg = (CModelessChild*)lParam;
		pMydlg->m_hdlgMe = hdlg;

		SetWindowLongPtr(hdlg, DWLP_USER, lParam);
	}

	DLGPROC_ret trueret = 0;

	if (pMydlg && uMsg != 0)
	{
		// note: WM_SETFONT precedes WM_INITDIALOG, so we need to check pMydlg.
		// note: After closing child dlg, we could see uMsg==0, weird.

		INT_PTR MsgRet = 0;
		Actioned_et actioned = pMydlg->DlgProc(uMsg, wParam, lParam, &MsgRet);

		trueret = actioned==Actioned_yes ? 
			SetDlgMsgResult(hdlg, uMsg, MsgRet) : DLGPROC_ActionedNo;
		return trueret;
	}
	else
	{
		if (!pMydlg) {
			vaDBG(_T("note: We see uMsg=%s when pMydlg is NULL."), ITCSv(uMsg, itc::WM_xxx));
		}
		if (uMsg == 0) {
			vaDBG(_T("note: We see uMsg==0 in DialogProc."));
		}
	}

//	vaDBG(_T("[BaseDlgProc]uMsg %s => %d"), ITCSv(uMsg, itc::WM_xxx), (int)trueret);
	return trueret;
}

CModelessChild::Actioned_et
CModelessChild::DlgProc(UINT uMsg, WPARAM wParam, LPARAM lParam, INT_PTR *pMsgRet)
{
	SETTLE_OUTPUT_PTR(INT_PTR, pMsgRet, 0);

	if (uMsg == WM_INITDIALOG)
	{
		vaDBG(_T("%s (hdlg=0x%08X) created."), msz_name, m_hdlgMe);
		
		*pMsgRet = AcceptDefaultFocus_TRUE;
		return Actioned_yes;
	}
	if (uMsg == WM_COMMAND)
	{
		assert(mr_ptrme_by_parent == this);

		UINT cmd = GET_WM_COMMAND_ID(wParam, lParam);
		if (cmd == IDCANCEL || cmd == IDOK)
		{
			assert(IsWindow(m_hdlgMe));
			::PostMessage(m_hdlgMe, m_WM_KillSelf, 0, 0);

			this->DlgClosing();
		}

		return Actioned_yes;
	}
	else if (uMsg == WM_DESTROY)
	{
		// [2025-06-02] Q: Why I can not see this?
		// Neither in debug-message of BaseDlgProc().
		return Actioned_no;
	}
	else if (uMsg == m_WM_KillSelf)
	{
		SetWindowLongPtr(m_hdlgMe, DWLP_USER, NULL);
		// -- This is important, it stops further dialog WM_xxx message from calling 
		// this virtual DlgProc(). See CModelessChild::BaseDlgProc().

		vaDBG(_T("%s DestroyWindow(hdlg=0x%08X)."), msz_name, m_hdlgMe);

		DestroyWindow(m_hdlgMe); 
		// -- Cannot merely use EndDialog(m_hdlgMe, 0);, that is the rule.
		// If merely use EndDialog(), the dlgbox vanishes but the HWND remains in system.

		// Tell parent that I have destroyed myself.
		mr_ptrme_by_parent = NULL;
		
		// delete this; // Shall not delete since v1.5.7
		// Instead:
		m_hdlgMe = NULL;
		m_hdlgParent = NULL;
		msz_name = NULL;
		msd_name = nullptr;

		return Actioned_yes;
	}
	else
		return Actioned_no;
}

#ifndef ModelessChild_DEBUG
#include <CHHI_vaDBG_show.h>
#endif


#endif // ModelessChild_IMPL

#endif
