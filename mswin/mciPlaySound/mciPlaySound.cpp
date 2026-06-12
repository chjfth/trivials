#include <windows.h>
#include <windowsx.h>
#include <ShlObj-winxp-patch.h> // avoid VC2015 WinXP warning
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

// For use in chj git-repo dir-tree
#define CHHI_ALL_IMPL

// These .h are from __chjcxx git-module
#include <vaDbgTs.h>
#include <mswin/utils_env.h>
#include <mswin/utils_wingui.h>
#include <mswin/JULayout2.h>
#include <chj_mishmash.h>


#include "CxxDialogBase.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


HINSTANCE g_hinstExe;

class MainDialog : public CxxDialogBase
{
public:
	MainDialog(LPCTSTR mystr);

	virtual INT_PTR DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam);

	MCIERROR OnMciNotify(HWND hdlg, UINT flags, LONG devid);

private:
	const TCHAR *m_mystr;
	int m_clicks;
};

MainDialog::MainDialog(LPCTSTR mystr)
{
	m_mystr = mystr;
	m_clicks = 0;
}

void MainDialog::OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	TCHAR szfile[MAX_PATH] = {};
	GetDlgItemText(hdlg, IDE_SOUND_FILE, szfile, MAX_PATH);
	Sdring mcicmd;
	MCIERROR mcierr = 0;

	TCHAR szErr[1024] = {};
	const int ccErr = ARRAYSIZE(szErr);

	HWND helog = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	switch(id) 
	{{
	case IDB_MCI_OPEN:
	{
		vaSdringAppendSelf(mcicmd, _T("open \"%s\" type MPEGVideo alias mysound"), szfile);
		mcierr = mciSendString(mcicmd, szErr, ccErr, nullptr);

		vaAppendText_mled(helog, _T("[%d] open MciRets: %s\r\n"), mcierr, szErr);

		break;
	}
	case IDB_MCI_PLAY:
	{
		vaSdringAppendSelf(mcicmd, _T("play mysound from 0 notify"), szfile);
		mcierr = mciSendString(mcicmd, szErr, ccErr, hdlg);

		vaAppendText_mled(helog, _T("[%d] play MciRets: %s\r\n"), mcierr, szErr);
		break;
	}
	case IDB_MCI_CLOSE:
	{
		vaSdringAppendSelf(mcicmd, _T("close mysound"), szfile);
		mcierr = mciSendString(mcicmd, szErr, ccErr, hdlg);

		vaAppendText_mled(helog, _T("[%d] close MciRets: %s\r\n"), mcierr, szErr);
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

	jul->AnchorControl(0,0, 100,0, IDE_SOUND_FILE);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
	jul->AnchorControl(0,100, 0,100, IDC_BUTTON1);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL MainDialog::OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	SetDlgItemText(hdlg, IDE_SOUND_FILE, _T("chime-56kbps-2sec.mp3"));

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

MCIERROR MainDialog::OnMciNotify(HWND hdlg, UINT flags, LONG devid)
{
	vaDbgTs(_T("OnMciNotify() flags=0x%X, devid=%d"), flags, devid);
	return 0;
}

INT_PTR MainDialog::DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       OnCommand);
	}

	if(uMsg==MM_MCINOTIFY)
	{
		OnMciNotify(hdlg, (UINT)wParam, (LONG)lParam);
	}

	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	MainDialog dlg(_T("Hello.\r\nPrivate string here."));
	dlg.DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL);

	return 0;
}
