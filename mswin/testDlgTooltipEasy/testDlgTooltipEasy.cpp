#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
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

#define CxxWindowSubclass_IMPL
#include <mswin/CxxWindowSubclass.h>

#define DlgTooltipEasy_IMPL
#define DlgTooltipEasy_DEBUG
#include <mswin/DlgTooltipEasy.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

const TCHAR* my_DlgtteGetText(void *userctx)
{
	HWND hUic = (HWND)userctx;

	static TCHAR stext[200];

	TCHAR szWndcls[80];
	GetClassName(hUic, szWndcls, ARRAYSIZE(szWndcls));

	_sntprintf_s(stext, _TRUNCATE, _T("EasyTooltip: {%s} hwnd=0x%X"), szWndcls, (UINT)hUic);

	return stext;
}


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st &prdata = *(DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
//	TCHAR textbuf[200];

	switch (id) 
	{{
	case IDB_AddEasyTooltip:
	{
		HWND hwndBtn = GetDlgItem(hdlg, IDB_AddEasyTooltip);
		Dlgtte_EnableTooltip(hwndBtn, my_DlgtteGetText, hwndBtn, nullptr, 0);
		HWND hwndEdt = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
		Dlgtte_EnableTooltip(hwndEdt, my_DlgtteGetText, hwndEdt, nullptr, 0);
		break;
	}
	case IDB_DelEasyTooltip:
	{
		HWND hwndBtn = GetDlgItem(hdlg, IDB_AddEasyTooltip);
		Dlgtte_RemoveTooltip(hwndBtn);
		HWND hwndEdt = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
		Dlgtte_RemoveTooltip(hwndEdt);
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
		testDlgTooltipEasy_VMAJOR, testDlgTooltipEasy_VMINOR, testDlgTooltipEasy_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);
	
	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, prdata.mystr);

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
