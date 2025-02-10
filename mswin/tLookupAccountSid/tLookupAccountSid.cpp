#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <Sddl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#include <EnsureClnup_mswin.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

void debuginfo_SID() { SID nullsid = {}; } // let debugger know `SID` struct

// todo: Test whether LookupAccoutSid returns different buffer sizes across each run.

#define NAMECHARS_MAX 1024
#define NumUnsigned FALSE

void Fill_MySID(HWND hdlg)
{
	HWND hedit = GetDlgItem(hdlg, IDE_QueryOutput);

	DWORD winerr = 0;
	HANDLE hToken = nullptr;
	BOOL succ = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
	Cec_PTRHANDLE cec_token = hToken;
	if(!succ)
	{
		winerr = GetLastError();
		vaAppendText_mled(hedit, _T("OpenProcessToken() winerr=%d"), winerr);
		return;
	}

	DWORD dwSize = 0;
	succ = GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
	if(!succ)
	{
		winerr = GetLastError();
		if(winerr!=ERROR_INSUFFICIENT_BUFFER)
		{
			vaAppendText_mled(hedit, _T("GetTokenInformation() winerr=%d"), winerr);
			return;
		}
	}

	TOKEN_USER *pTokenUser = (TOKEN_USER *)new char[dwSize];
	Cec_cxx_delete cec_tokenuser = pTokenUser;
	succ = GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize);

	TCHAR *sidtext = nullptr;
	succ = ConvertSidToStringSid(pTokenUser->User.Sid, &sidtext);
	Cec_LocalFree cec_sidtext = sidtext;
	if(!succ)
	{
		vaAppendText_mled(hedit, _T("ConvertSidToStringSid() winerr=%d"), winerr);
		return;
	}

	SetDlgItemText(hdlg, IDE_SidInput, sidtext);

	SetDlgItemText(hdlg, IDE_QueryOutput, 
		_T("\"SID to Query\" has been filled according to your current logon user.\r\n")
		_T("Press LookupAccountSid button to query the SID for its name (maybe user-name or group-name).\r\n")
		_T("\r\n")
		_T("You can use assign a small buffer size and see how the API reacts.")
		);
}

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDE_SidInput);
	jul->AnchorControl(0,0, 0,0, IDS_groubox1);
	jul->AnchorControl(0,0, 100,100, IDE_QueryOutput);
	jul->AnchorControl(50,100, 50,100, IDB_Query);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

void Dlg_InitUI(HWND hdlg)
{
	Fill_MySID(hdlg);

	SetDlgItemInt(hdlg, IDE_cchName,    NAMECHARS_MAX, NumUnsigned);
	SetDlgItemInt(hdlg, IDE_cchDomName, NAMECHARS_MAX, NumUnsigned);
}

void Do_Query(HWND hdlg)
{
	HWND heo = GetDlgItem(hdlg, IDE_QueryOutput);

	DWORD winerr = 0;
	TCHAR sidtext[256] = {};
	SID *psid = nullptr;
	GetDlgItemText(hdlg, IDE_SidInput, sidtext, ARRAYSIZE(sidtext));
	BOOL succ = ConvertStringSidToSid(sidtext, (PSID*)&psid);
	Cec_LocalFree cec_sid = psid;
	if(!succ)
	{
		winerr = GetLastError();
		vaSetWindowText(heo,
			_T("ConvertStringSidToSid(\"%s\") failed with WinErr=%d"),
			sidtext, winerr);
		return;
	}

	DWORD ciName = 0, ciDomName = 0;
	BOOL isok = FALSE;
	ciName = GetDlgItemInt(hdlg, IDE_cchName, &isok, NumUnsigned);
	if(!isok || ciName>NAMECHARS_MAX)
	{
		vaSetWindowText(heo,
			_T("ERROR: cchName must be a value between 0 ~ %d"), NAMECHARS_MAX);
		return;
	}

	ciDomName = GetDlgItemInt(hdlg, IDE_cchDomName,&isok, NumUnsigned);
	if(!isok || ciDomName>NAMECHARS_MAX)
	{
		vaSetWindowText(heo,
			_T("ERROR: cchReferencedDomainName must be a value between 0 ~ %d"), NAMECHARS_MAX);
		return;
	}

	DWORD coName = ciName, coDomName = ciDomName;
	
	TCHAR szName[NAMECHARS_MAX] = {}, szDomName[NAMECHARS_MAX] = {};
	SID_NAME_USE sidtype = SidTypeInvalid;
	DWORD succ_lookup = LookupAccountSid(NULL, psid, 
		szName, &coName,
		szDomName, &coDomName,
		&sidtype);

	if(!succ_lookup)
	{
		winerr = GetLastError();
		vaSetWindowText(heo, 
			_T("ERROR: LookupAccountSid() fail! WinErr=%d\r\n\r\n"), winerr);
		
		// Still show coName/cchReferencedDomainName output value.

		vaAppendText_mled(heo, 
			_T("cchName output: %d\r\n")
			_T("cchReferencedDomainName output: %d\r\n")
			,
			coName,
			coDomName);
		return;
	}
	
	//
	// Success report:
	//
	vaSetWindowText(heo, _T(""));

	vaAppendText_mled(heo, 
		_T("cchName(%d TCHARs): %s\r\n")
		_T("cchReferencedDomainName(%d TCHARs): %s\r\n")
		_T("SID_NAME_USE: %d")
		,
		coName, szName,
		coDomName, szDomName,
		sidtype);
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	//	TCHAR textbuf[200];

	switch (id) 
	{{
	case IDB_Query:
		{
			Do_Query(hdlg);
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

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	vaSetWindowText(hdlg, _T("tLookupAccountSid v%d.%d.%d"), 
		tLookupAccountSid_VMAJOR, tLookupAccountSid_VMINOR, tLookupAccountSid_VPATCH);
	
	Dlg_EnableJULayout(hdlg);

	Dlg_InitUI(hdlg);

	SetFocus(GetDlgItem(hdlg, IDB_Query));
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
