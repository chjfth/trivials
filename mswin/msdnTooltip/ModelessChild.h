#ifndef __ModelessChild_h_202502512_
#define __ModelessChild_h_202502512_

// This .h can be a common library, not just used in a single EXE project.

#include <tchar.h>
#include <windows.h>

#include <commdefs.h>
#include <mswin/mswinClarify.h>
#include <SdringTCHAR.h>

class CModelessChild
{
	// This class helps the parent-dialog manage this object as modeless child-dialog.

public:
	CModelessChild(CModelessChild** pptrme_by_parent) : mr_ptrme_by_parent(*pptrme_by_parent)
	{}

	BOOL CreateTheDialog(const TCHAR *name,
		HINSTANCE hInstExe, int dlg_resid, HWND hdlgParent);

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

protected:
	Sdring msd_name;
	const TCHAR *msz_name = nullptr;
	HWND m_hdlgMe = nullptr;

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
#define vaDBG(...) // make vaDBG empty, no debugging message
#else
#include <mswin/winuser.itc.h>
#endif


BOOL CModelessChild::CreateTheDialog(const TCHAR *name, 
	HINSTANCE hInstExe, int dlg_resid, HWND hdlgParent)
{
	msd_name = name;
	msz_name = msd_name;

	m_hdlgMe = CreateDialogParam(hInstExe, MAKEINTRESOURCE(dlg_resid), hdlgParent,
		StartDlgProc,
		(LPARAM)this // dlgparam
	);
	
	mr_ptrme_by_parent = this;
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
		
		*pMsgRet = ACCEPT_DEFAULT_FOCUS;
		return Actioned_yes;
	}
	if (uMsg == WM_COMMAND)
	{
		assert(mr_ptrme_by_parent == this);

		UINT cmd = GET_WM_COMMAND_ID(wParam, lParam);
		if (cmd == IDCANCEL)
		{
			assert(IsWindow(m_hdlgMe));
			::PostMessage(m_hdlgMe, m_WM_KillSelf, 0, 0);
		}

		return Actioned_yes;
	}
	else if (uMsg == WM_DESTROY)
	{
		// [2025-05-10] Q: Why I can not see this?
		// Neither in debug-message of BaseDlgProc().
		return Actioned_no;
	}
	else if (uMsg == m_WM_KillSelf)
	{
		SetWindowLongPtr(m_hdlgMe, DWLP_USER, NULL);
		// -- This is important, it stops further dialog WM_xxx message from calling 
		// this virtual DlgProc(). See CModelessChild::BaseDlgProc().

		vaDBG(_T("%s (hdlg=0x%08X) EndDialog()."), msz_name, m_hdlgMe);

		EndDialog(m_hdlgMe, 0);

		// Tell parent that I have destroyed myself.
		mr_ptrme_by_parent = NULL;
		delete this;

		return Actioned_yes;
	}
	else
		return Actioned_no;
}

#ifndef ModelessChild_DEBUG
#undef vaDBG       // revoke empty effect
#endif


#endif // ModelessChild_IMPL

#endif
