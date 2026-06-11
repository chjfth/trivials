#ifndef __CxxDialogBase_h_20260313_
#define __CxxDialogBase_h_20260313_

#include <assert.h>
#include <tchar.h>
#include <windows.h>

class CxxDialogBase
{
public:
	CxxDialogBase()
	{
		m_hwndDlg = NULL;
	}

public:
	// User overrides this virtual function to do dlgbox message processing.
	virtual INT_PTR DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

public:
	INT_PTR DialogBoxParam(HINSTANCE hInstance, LPCTSTR lpTemplateName, HWND hWndParent)
	{
		return ::DialogBoxParam(hInstance, 
			lpTemplateName, hWndParent, s_DialogProc, (LPARAM)this);
	}

	HWND CreateModeless(HINSTANCE hInstance, LPCTSTR lpTemplateName, HWND hWndParent)
	{
		HWND hdlg = ::CreateDialogParam(hInstance, 
			lpTemplateName, hWndParent, s_DialogProc, (LPARAM)this);
		assert(hdlg == m_hwndDlg);
		return m_hwndDlg;
	}

	HWND GetHwnd(){ return m_hwndDlg; }

	void Show(bool isShow, int x, int y)
	{
		ShowWindow(m_hwndDlg, isShow?SW_SHOW:SW_HIDE);
		RECT rc = {};
		GetWindowRect(m_hwndDlg, &rc);
		MoveWindow(m_hwndDlg, x, y, rc.right-rc.left, rc.bottom-rc.top, TRUE);
	}

protected:
	HWND m_hwndDlg;

private:
	static INT_PTR CALLBACK s_DialogProc(HWND hdlg,
		UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		CxxDialogBase *pdlg = NULL;
		if(uMsg==WM_INITDIALOG)
		{
			pdlg = (CxxDialogBase*)lParam;
			SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)pdlg);

			pdlg->m_hwndDlg = hdlg;
		}
		else
		{
			pdlg = (CxxDialogBase*)GetWindowLongPtr(hdlg, DWLP_USER);
		}

		INT_PTR ret = 0;

		if(pdlg)
		{
			// We need to check non-NULL, bcz some WM_xxx precedes WM_INITDIALOG
			// e.g. WM_SETFONT .

			// Call virtual function.
			ret = pdlg->DialogProc(hdlg, uMsg, wParam, lParam);
		}

		return ret;
	}

};


#endif
