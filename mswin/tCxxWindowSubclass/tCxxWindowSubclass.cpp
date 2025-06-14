#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <crtdbg.h>

#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#include <utility>       // to have std::forward
#include <CxxVerCheck.h> // to see CXX11_OR_NEWER

#ifdef CXX11_OR_NEWER
template<typename... Args>
void vaDBG(Args&&... args) // forwards all arguments
{
	vaDbgTs(std::forward<Args>(args)...);
}
#else
void vaDBG(...)
{
	vaDbgTs(_T("Not C++11 compiled, vaDBG() is nullop."));
};
#endif


#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#include <mswin/WinUser.itc.h>
using namespace itc;

#define CxxWindowSubclass_IMPL
#define CxxWindowSubclass_DEBUG
#include <mswin/CxxWindowSubclass.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

class CEditboxPeeker : public CxxWindowSubclass
{
public:
	CEditboxPeeker()
	{}

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		vaDbgTs(_T("{0x%X} sees %s"), hwnd, ITCSv(uMsg, WM_xxx));

		return DefSubclassProc(hwnd, uMsg, wParam, lParam);
	}

private:
//	TCHAR m_szClassname[80];
};

//////////////////////////////////////////////////////////////////////////

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st &prdata = *(DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	
	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	switch (id) 
	{{
	case IDB_StartSubclass:
	{
		vaDbgTs(_T("IDB_StartSubclass dump memleak:"));
		_CrtDumpMemoryLeaks();

		CEditboxPeeker *ptm = CxxWindowSubclass::FetchCxxobjFromHwnd<CEditboxPeeker>(
			hedit, _T("sig_CEditboxPeeker"), TRUE, &err);

		assert(ptm);

		break;
	}
	case IDB_StopSubclass:
	{
		CEditboxPeeker *ptm = CxxWindowSubclass::FetchCxxobjFromHwnd<CEditboxPeeker>(
			hedit, _T("sig_CEditboxPeeker"), FALSE, &err);
		
		if(ptm)
		{
			ptm->DetachHwnd(true);
		}
		else
		{
			vaMsgBox(hdlg, MB_OK|MB_ICONEXCLAMATION, NULL, _T("Invalid op, CEditboxPeeker not exists."));
		}

		vaDbgTs(_T("IDB_StopSubclass dump memleak:"));
		_CrtDumpMemoryLeaks();

		break;
	}
	case IDOK:
	case IDCANCEL:
	{
		EndDialog(hdlg, id);
		break;
	}
	}}
}

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
	jul->AnchorControl(50,100, 50,100, IDC_BUTTON1);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st &prdata = *(DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)&prdata);
	
	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		tCxxWindowSubclass_VMAJOR, tCxxWindowSubclass_VMINOR, tCxxWindowSubclass_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);
	
	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
		_T("Press [Start] button to subclass this editbox.")
		_T("\r\n\r\n")
		_T("Use DbgView.exe, you can see window-messages for this editbox and the button.")
		);

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR WINAPI UserDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	DlgPrivate_st dlgdata = { _T("Hello.\r\nPrivate string here.") };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, (LPARAM)&dlgdata);

	return 0;
}
