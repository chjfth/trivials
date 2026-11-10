// Context: Evclip 20261110.s1 

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ShellAPI.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"

#include "iversion.h"

#define CHHI_ALL_IMPL

// These .h are from __chjcxx git-module
#include <msvc_extras.h>
#include <vaDbgTs.h>
#include <mswin/utils_env.h>
#include <mswin/utils_wingui.h>
#include <mswin/JULayout2.h>
#include <makeTsdring.h>

#include "CxxDialogBase.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


class MainDialog : public CxxDialogBase
{
public:
	MainDialog(LPCTSTR mystr);

	virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	// Note: This OnXXX functions needs to have `HWND dlg` as first param, because 
	// WindowsX.h's HANDLE_WM_xxx(HANDLE_WM_INITDIALOG, HANDLE_WM_COMMAND etc) requires it. 
	BOOL OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam);
	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);

	Sdring Get_FOLDERID_Captures();

private:
	const TCHAR *m_mystr;
	int m_clicks;
};

MainDialog::MainDialog(LPCTSTR mystr)
{
	m_mystr = mystr;
	m_clicks = 0;
}

Sdring MainDialog::Get_FOLDERID_Captures()
{
	HWND hedit = GetDlgItem(m_hwndDlg, IDC_EDIT_LOGMSG);

	PWSTR wpath = nullptr;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_AppCaptures, 0, nullptr, &wpath);
	if(hr!=S_OK)
	{
		vaAppendLog_mled(hedit,
			_T("SHGetKnownFolderPath(FOLDERID_AppCaptures) fail with hr=0x%X"), hr);
		return nullptr;
	}

	Sdring tpath = makeTsdring(wpath);
	CoTaskMemFree(wpath);

	vaAppendLog_mled(hedit, _T("%s"), tpath.c_str());

	return tpath;
}

void MainDialog::OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		Get_FOLDERID_Captures();
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

BOOL MainDialog::OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	assert(hdlg==m_hwndDlg);
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	vaSetWindowText(hdlg, _T("chkGameBar v%d.%d.%d"), 
		chkGameBarDir_VMAJOR, chkGameBarDir_VMINOR, chkGameBarDir_VPATCH);
	
	//SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, m_mystr);

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR MainDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	HWND hdlg = m_hwndDlg;
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       OnCommand);
	}
	return FALSE; // Let DlgMan do default for current message.
}


int in_WinMain(int argc, const TCHAR *argv[])
{
	HINSTANCE hinstExe = GetModuleHandle(NULL);

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	MainDialog dlg(_T("Hello.\r\nPrivate string here."));
	dlg.DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL);

	return 0;
}

int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	int argc = __argc;
#ifdef UNICODE
	PCTSTR* argv = (PCTSTR*) CommandLineToArgvW(GetCommandLine(), &argc);
#else
	PCTSTR* argv = (PCTSTR*) __argv;
#endif
	// Use argc and argv[] below, just like what traditional main() does.

#ifdef __msvc_extras_h_
	MSVCRT_MemCheckStart(foo);
#endif

	int ret = in_WinMain(argc, argv);

#ifdef __msvc_extras_h_
	bool isleak = MSVCRT_MemCheckEnd_IsLeak(foo);
	if (isleak) {
		OutputDebugString(_T("Suspect memleak in my C++ code.\n"));
		return ret ? 4 : ret;
	}
	else {
		OutputDebugString(_T("No memleak.\n"));
		return ret;
	}
#else
	return ret;
#endif

}
