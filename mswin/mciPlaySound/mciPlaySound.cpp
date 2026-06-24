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

static void LogMciErr(HWND helog, MCIERROR mcierr, const TCHAR *szErr)
{
	if(mcierr)
	{
		if (szErr && szErr[0])
			vaAppendLog_mled(helog, _T("[ret] MCIERR=%d (%s)"), mcierr, szErr);
		else
			vaAppendLog_mled(helog, _T("[ret] MCIERR=%d."), mcierr);
	}
	else
		vaAppendLog_mled(helog, _T("[ret] success"));
}

static void LogAndRunMci(HWND helog, const TCHAR *mcicmd, HWND hwndNotify)
{
	TCHAR szErr[1024] = {};
	const int ccErr = ARRAYSIZE(szErr);

	vaAppendLog_mled(helog, _T("[run] '%s'"), mcicmd);
	MCIERROR mcierr = mciSendString(mcicmd, szErr, ccErr, hwndNotify);
	LogMciErr(helog, mcierr, szErr);
}

void MainDialog::OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	Sdring mcicmd;
	MCIERROR mcierr = 0;
	HWND helog = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	if(codeNotify==EN_SETFOCUS)
	{
		static const FocusToDefaultBtn_st smap[] = 
		{
			{ IDE_SOUND_FILE, IDB_MCI_OPEN },
			{ IDE_MCI_COMMAND, IDB_EXECUTE },
		};
		Set_DlgDefaultButton_byFocusId(hdlg, id, smap, ARRAYSIZE(smap));
	}

	switch(id) 
	{{
	case IDB_MCI_OPEN:
	{
		Sdring sdrfile = sdrGetDlgItemText(hdlg, IDE_SOUND_FILE);
		vaSdringAppendSelf(mcicmd, _T("open \"%s\" type MPEGVideo alias mysound"), sdrfile.c_str());

		LogAndRunMci(helog, mcicmd, hdlg);
		
		SetDlgItemText(hdlg, IDE_MCI_COMMAND, mcicmd);
		break;
	}
	case IDB_MCI_CLOSE:
	{
		vaSdringAppendSelf(mcicmd, _T("close mysound"));
		
		LogAndRunMci(helog, mcicmd, hdlg);
		
		SetDlgItemText(hdlg, IDE_MCI_COMMAND, mcicmd);
		break;
	}
	case IDB_PLAY:
	{
		vaSdringAppendSelf(mcicmd, _T("play mysound from 0 notify"));

		LogAndRunMci(helog, mcicmd, hdlg);
		
		SetDlgItemText(hdlg, IDE_MCI_COMMAND, mcicmd);
		break;
	}
	case IDB_PLAY_REPEAT:
	{
		vaSdringAppendSelf(mcicmd, _T("play mysound from 0 repeat"));

		LogAndRunMci(helog, mcicmd, hdlg);
		
		SetDlgItemText(hdlg, IDE_MCI_COMMAND, mcicmd);
		break;
	}
	case IDB_STOP:
	{
		vaSdringAppendSelf(mcicmd, _T("stop mysound"));

		LogAndRunMci(helog, mcicmd, hdlg);
		
		SetDlgItemText(hdlg, IDE_MCI_COMMAND, mcicmd);
		break;
	}
	case IDB_EXECUTE:
	{
		mcicmd = sdrGetDlgItemText(hdlg, IDE_MCI_COMMAND);

		LogAndRunMci(helog, mcicmd, hdlg);
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
	jul->AnchorControls(100,0, 100,0, IDB_MCI_OPEN, IDB_MCI_CLOSE, -1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);

	jul->AnchorControls(0,100, 0,100, IDS_MCI_COMMAND, 
		IDB_PLAY, IDB_PLAY_REPEAT, IDB_STOP,
		-1);
	jul->AnchorControls(0,100, 100,100, IDE_MCI_COMMAND, -1);
	jul->AnchorControls(100,100, 100,100, IDB_EXECUTE, -1);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL MainDialog::OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	vaSetWindowText(hdlg, _T("mciPlaySound v%d.%d.%d"),
		mciPlaySound_VMAJOR, mciPlaySound_VMINOR, mciPlaySound_VPATCH);

	SetDlgItemText(hdlg, IDE_SOUND_FILE, _T("chime-2sec.mp3"));

	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
		_T("Hint: [MCI open] first, then [play].\r\n"));

	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
	Edit_LimitText(hedit, 1024 * 1024); // enlarge max text

	Dlg_EnableJULayout(hdlg);

	Set_DlgDefaultButton(hdlg, IDB_MCI_OPEN);
	SetFocus(GetDlgItem(hdlg, IDE_SOUND_FILE));
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
