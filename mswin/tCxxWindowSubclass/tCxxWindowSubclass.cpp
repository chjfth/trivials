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

#include <CHHI_vaDBG_is_vaDbgTs.h>

#define JULayout2_IMPL
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
		vaDbgTs(_T("[cxxobj %p] Hwnd=0x%X sees %s"), this, hwnd, ITCSv(uMsg, WM_xxx));

		return DefSubclassProc(hwnd, uMsg, wParam, lParam);
	}

private:
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
	
	static int s_UnnamedCount = 0;

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	const TCHAR *SIGSTR = _T("sig_CEditboxPeeker");
	// -- Note: In order to DetachHwnd() later manually, user need to explicitly assign a sigstr.

	switch (id) 
	{{
	case IDB_StartSubclass:
	{
//		vaDbgTs(_T("IDB_StartSubclass dump memleak:"));
//		_CrtDumpMemoryLeaks();

		CEditboxPeeker *psp = CxxWindowSubclass::FetchCxxobjFromHwnd<CEditboxPeeker>(
			hedit, SIGSTR, TRUE, &err);
		// -- psp: pointer to subclass-peeker

		assert(psp);

		if(err==CxxWindowSubclass::E_Success)
		{
			SetDlgItemText(hdlg, IDS_NamedState, _T("Started"));

			vaMsgBox(hdlg, MB_OK|MB_ICONINFORMATION, NULL, 
				_T("Start success. \r\n\r\n")
				_T("Cxxobj address: %p")
				, psp);
		}
		else if(err==CxxWindowSubclass::E_Existed)
		{
			vaMsgBox(hdlg, MB_OK|MB_ICONEXCLAMATION, NULL,
				_T("Already started, returning old CxxObj.\r\n\r\n")
				_T("CxxObj address: %p")
				, psp);
		}
		else
			assert(0);

		break;
	}
	case IDB_StopSubclass:
	{
		CEditboxPeeker *psp = CxxWindowSubclass::FetchCxxobjFromHwnd<CEditboxPeeker>(
			hedit, SIGSTR, FALSE, &err);
		
		if(psp)
		{
			psp->DetachHwnd(true);

			SetDlgItemText(hdlg, IDS_NamedState, _T("Stopped"));
		}
		else
		{
			vaMsgBox(hdlg, MB_OK|MB_ICONEXCLAMATION, NULL, _T("Invalid op, CEditboxPeeker not exists."));
		}

//		vaDbgTs(_T("IDB_StopSubclass dump memleak:")); 	
//		_CrtDumpMemoryLeaks();

		break;
	}
	case IDB_CreateUnnamed:
	{
		// This time, we use nullptr for subclassing-sigstr.
		// Each time CxxWindowSubclass libcode will generate one for us, crazy reproduction.

		CEditboxPeeker *psp2 = CxxWindowSubclass::FetchCxxobjFromHwnd<CEditboxPeeker>(
			hedit, nullptr, TRUE, &err);

		if(psp2)
		{
			vaMsgBox(hdlg, MB_OK, NULL, 
				_T("One more subclass-instance created. \r\n\r\n")
				_T("CxxObj address: %p")
				, psp2);

			s_UnnamedCount++;

			vaSetDlgItemText(hdlg, IDS_count, _T("count: %d"), s_UnnamedCount);
		}
		else
		{	// Not likely to see, unless no-mem.
			vaMsgBox(hdlg, MB_OK|MB_ICONEXCLAMATION, NULL, 
				_T("[ERROR] CxxWindowSubclass::ReCode_et = %d"), err);
		}

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




bool test(HWND hdlg)
{
	CxxWindowSubclass::ReCode_et err1, err2, errX;
	CEditboxPeeker *psp1 = new CEditboxPeeker;
	CEditboxPeeker *psp2 = new CEditboxPeeker;

	err1 = psp1->AttachHwnd(hdlg, _T("ChjSig1"));
	errX = psp2->AttachHwnd(hdlg, _T("ChjSig1")); // got E_CxxObjConflict

	err2 = psp2->AttachHwnd(hdlg, _T("ChjSig2")); // ok

	return true;
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

	SetDlgItemText(hdlg, IDS_NamedState, _T("Stopped"));

//	test(hdlg);

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
