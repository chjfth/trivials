#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <Sddl.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#include <EnsureClnup_mswin.h>
#include <mswin/WinError.itc.h>
#include <mswin/winnt.itc.h>

using namespace itc;

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

void debuginfo_SID() { SID nullsid = {}; } // let debugger know `SID` struct

#define NAMECHARS_MAX 1024
#define NumSigned TRUE

void Fill_MySID(HWND hdlg)
{
	HWND hedit = GetDlgItem(hdlg, IDE_QueryOutput);

	DWORD winerr = 0;
	HANDLE hToken = nullptr;
	BOOL succ = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
	CEC_PTRHANDLE cec_token = hToken;
	if(!succ)
	{
		winerr = GetLastError();
		vaAppendText_mled(hedit, _T("OpenProcessToken() winerr=%s"), ITCSv(winerr, WinError));
		return;
	}

	DWORD dwSize = 0;
	succ = GetTokenInformation(hToken, TokenUser, NULL, 0, &dwSize);
	if(!succ)
	{
		winerr = GetLastError();
		if(winerr!=ERROR_INSUFFICIENT_BUFFER)
		{
			vaAppendText_mled(hedit, _T("GetTokenInformation() winerr=%s"), ITCSv(winerr, WinError));
			return;
		}
	}

	TOKEN_USER *pTokenUser = (TOKEN_USER *)new char[dwSize];
	CEC_raw_delete cec_tokenuser = pTokenUser;
	succ = GetTokenInformation(hToken, TokenUser, pTokenUser, dwSize, &dwSize);

	TCHAR *sidtext = nullptr;
	succ = ConvertSidToStringSid(pTokenUser->User.Sid, &sidtext);
	CEC_LocalFree cec_sidtext = sidtext;
	if(!succ)
	{
		vaAppendText_mled(hedit, _T("ConvertSidToStringSid() winerr=%s"), ITCSv(winerr, WinError));
		return;
	}

	SetDlgItemText(hdlg, IDE_SidInput, sidtext);

	SetDlgItemText(hdlg, IDE_QueryOutput, 
		_T("\"SID to Query\" has been filled according to your current logon user.\r\n")
		_T("Press LookupAccountSid button to query the SID for its name (maybe user-name or group-name).\r\n")
		_T("\r\n")
		_T("You can assign a small buffer size and see how the API reacts.")
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

	SetDlgItemInt(hdlg, IDE_cchName,    NAMECHARS_MAX, NumSigned);
	SetDlgItemInt(hdlg, IDE_cchDomName, NAMECHARS_MAX, NumSigned);
}

void Print_IntValueError(HWND hedt, const TCHAR *pnameBuffer, const TCHAR *pnameBuflen)
{
	vaAppendText_mled(hedt,
		_T("ERROR: Input value for %s invalid.\r\n") // 1
		_T("Valids are:\r\n") 
		_T("[A] Value between 0 ~ %d (example: 10). So %s is passed a non-NULL buffer pointer, and %s is passed 10.\r\n") // 2,3
		_T("[B] Leave blank. Then %s=NULL and %s=0 .\r\n") // 4,5
		_T("[C] A negative value(example: -10). Then %s=NULL, and %s=10 .\r\n") // 6,7
		, 
		pnameBuflen, // 1
		NAMECHARS_MAX, pnameBuffer, pnameBuflen, // 2,3
		pnameBuffer, pnameBuflen, // 4,5
		pnameBuffer, pnameBuflen // 6,7
		);
}

static TCHAR *Set_BufPtrAndLen(TCHAR rawtext[], int *pretlen)
{
	// For case [B], blank input
	if(rawtext[0]=='\0')
	{
		*pretlen = 0;
		return NULL;
	}

	int num = _ttoi(rawtext);
	if(num>=0)
	{
		// case [A]
		*pretlen = num;
		return rawtext;
	}
	else
	{
		// case [C]
		*pretlen = -num;
		return NULL;
	}
}

static bool Check_InputValid(const TCHAR *pbuf, int buflen, 
	HWND hedtMsg, const TCHAR *pnameBuffer, const TCHAR *pnameBuflen)
{
	if(buflen > NAMECHARS_MAX)
	{
		Print_IntValueError(hedtMsg, pnameBuffer, pnameBuflen);
		return false;
	}

	if(pbuf==NULL)
	{
		// Explicitly tell the user: You provided "valid" but weird input.
		vaAppendText_mled(hedtMsg,
			_T("Input: %s=NULL, %s=%d\r\n"),
			pnameBuffer, pnameBuflen, buflen
			);
	}

	return true;
}

void Do_Query(HWND hdlg)
{
	HWND heo = GetDlgItem(hdlg, IDE_QueryOutput);

	DWORD winerr = 0;
	TCHAR sidtext[256] = {};
	SID *psid = nullptr;
	GetDlgItemText(hdlg, IDE_SidInput, sidtext, ARRAYSIZE(sidtext));
	BOOL succ = ConvertStringSidToSid(sidtext, (PSID*)&psid);
	CEC_LocalFree cec_sid = psid;
	if(!succ)
	{
		winerr = GetLastError();
		vaSetWindowText(heo,
			_T("ConvertStringSidToSid(\"%s\") failed with WinErr=%s"), 
			sidtext, ITCSv(winerr, WinError));
		return;
	}

	vaSetWindowText(heo, _T("")); // clear output text

	TCHAR szName[NAMECHARS_MAX] = {}, szDomName[NAMECHARS_MAX] = {};
	TCHAR *p1 = szName, *p2 = szDomName;
	int n1 = 0, n2 = 0;

	GetDlgItemText(hdlg, IDE_cchName, szName, NAMECHARS_MAX);
	GetDlgItemText(hdlg, IDE_cchDomName, szDomName, NAMECHARS_MAX);

	p1 = Set_BufPtrAndLen(szName, &n1);
	p2 = Set_BufPtrAndLen(szDomName, &n2);

	if(!Check_InputValid(p1, n1, heo, _T("lpName"), _T("cchName")))
		return;
	if(!Check_InputValid(p2, n2, heo, _T("lpReferencedDomainName"), _T("cchReferencedDomainName")))
		return;

	SID_NAME_USE sidtype = SidTypeInvalid;
	DWORD succ_lookup = LookupAccountSid(NULL, psid, 
		p1, (DWORD*)&n1, // account-name
		p2, (DWORD*)&n2, // domain-name
		&sidtype);

	if(!succ_lookup)
	{
		winerr = GetLastError();
		vaAppendText_mled(heo, 
			_T("ERROR: LookupAccountSid() fail! WinErr=%s\r\n\r\n"), ITCSv(winerr, WinError));
		
		// Still show coName/cchReferencedDomainName output value,
		// so that we can know how much buffer shall we prepare.

		vaAppendText_mled(heo, 
			_T("cchName output: %d\r\n")
			_T("cchReferencedDomainName output: %d\r\n")
			,
			n1,
			n2);
		return;
	}
	
	//
	// Success report:
	//

	vaAppendText_mled(heo, 
		_T("Success.\r\n")
		_T("cchName(%d TCHARs): %s\r\n")
		_T("cchReferencedDomainName(%d TCHARs): %s\r\n")
		_T("SID_NAME_USE: %s")
		,
		n1, szName,
		n2, szDomName,
		ITCSv(sidtype, SidTypeXXX));
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
