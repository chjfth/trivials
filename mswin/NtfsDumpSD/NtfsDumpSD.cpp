#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <Shlobj.h>
#include <Aclapi.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#define util_WinSecuAPI_IMPL
#include <mswin/util_WinSecuAPI.h>

#include <mswin/WinError.itc.h>
using namespace itc;

#include "iversion.h"

#include "../utils.h"
#include "ch10-DumpSD.h" // for g_dbgbuf[]

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

enum FileOrDir_et 
{
	FodUnknown = 0,
	FodFile = 1,
	FodDir = 2,
};

struct ITR_st
{
	FileOrDir_et fod;
};


TCHAR* InterpretRights(DWORD rights, void *userctx)
{
	const int indents = 4;
	const int BufChars = 4000;
	TCHAR *pbuf = new TCHAR[BufChars];
	pbuf[0] = '\0';

	// Check for special ALL rights cases(to avoid verbose output)
	//
	if(rights==0x1F01FF)
	{
		_sntprintf_s(pbuf, BufChars, _TRUNCATE,
			_T("%*s (STANDARD_RIGHTS_ALL | FILE_ALL_ACCESS)\r\n"), indents, _T(""));
	
		return pbuf;
	}

#define DUMP_ONE_BIT_PERLINE(bitnames) \
	if( (rights & (bitnames)) == (bitnames) ) \
	_sntprintf_s(pbuf, BufChars, _TRUNCATE, _T("%s%*s (0x%08X) %s\r\n"), pbuf, indents, _T(""), \
	(bitnames), _T(#bitnames));

	ITR_st &ctx = *(ITR_st*)userctx;

	DUMP_ONE_BIT_PERLINE(GENERIC_READ);
	DUMP_ONE_BIT_PERLINE(GENERIC_WRITE);
	DUMP_ONE_BIT_PERLINE(GENERIC_EXECUTE);
	DUMP_ONE_BIT_PERLINE(GENERIC_ALL);

	DUMP_ONE_BIT_PERLINE(SYNCHRONIZE);
	DUMP_ONE_BIT_PERLINE(WRITE_OWNER);
	DUMP_ONE_BIT_PERLINE(WRITE_DAC);
	DUMP_ONE_BIT_PERLINE(READ_CONTROL);
	DUMP_ONE_BIT_PERLINE(DELETE);

	DUMP_ONE_BIT_PERLINE(FILE_WRITE_ATTRIBUTES); // 0x100
	DUMP_ONE_BIT_PERLINE(FILE_READ_ATTRIBUTES);  // 0x80
	DUMP_ONE_BIT_PERLINE(FILE_WRITE_EA);         // 0x10
	DUMP_ONE_BIT_PERLINE(FILE_READ_EA);          // 0x08

	if(ctx.fod==FodDir)
	{
		DUMP_ONE_BIT_PERLINE(FILE_DELETE_CHILD);     // 0x40
		DUMP_ONE_BIT_PERLINE(FILE_TRAVERSE);         // 0x20

		DUMP_ONE_BIT_PERLINE(FILE_ADD_SUBDIRECTORY); // 0x04
		DUMP_ONE_BIT_PERLINE(FILE_ADD_FILE);         // 0x02
		DUMP_ONE_BIT_PERLINE(FILE_LIST_DIRECTORY);   // 0x01
	}
	else if(ctx.fod==FodFile)
	{
		                                             // 0x40 (none for file)
		DUMP_ONE_BIT_PERLINE(FILE_EXECUTE);          // 0x20

		DUMP_ONE_BIT_PERLINE(FILE_APPEND_DATA);      // 0x04
		DUMP_ONE_BIT_PERLINE(FILE_WRITE_DATA);       // 0x02
		DUMP_ONE_BIT_PERLINE(FILE_READ_DATA);        // 0x01
	}
	else
	{
		assert(ctx.fod==FodUnknown);

		// This can occur if the path to check fails on GetFileAttributes(),
		// probably due to ERROR_ACCESS_DENIED.

		DUMP_ONE_BIT_PERLINE(FILE_DELETE_CHILD);                        // 0x40
		DUMP_ONE_BIT_PERLINE(FILE_TRAVERSE | FILE_EXECUTE);             // 0x20

		DUMP_ONE_BIT_PERLINE(FILE_ADD_SUBDIRECTORY | FILE_APPEND_DATA); // 0x04
		DUMP_ONE_BIT_PERLINE(FILE_ADD_FILE | FILE_WRITE_DATA);          // 0x02
		DUMP_ONE_BIT_PERLINE(FILE_LIST_DIRECTORY | FILE_READ_DATA);     // 0x01
	}

	// Four composite right names:
	DUMP_ONE_BIT_PERLINE(FILE_ALL_ACCESS);
	DUMP_ONE_BIT_PERLINE(FILE_GENERIC_READ);
	DUMP_ONE_BIT_PERLINE(FILE_GENERIC_WRITE);
	DUMP_ONE_BIT_PERLINE(FILE_GENERIC_EXECUTE);

	DWORD unknown_bits = rights & ~0xF01F01FF;
	if(unknown_bits)
	{
		_sntprintf_s(pbuf, BufChars, _TRUNCATE, 
			_T("%s%*s Weird: Meet unknown access-right-bits: 0x%08X\r\n"), pbuf, indents, _T(""), 
			unknown_bits);
	}

	return pbuf;
}

void do_DumpSD(HWND hdlg, const TCHAR *path, PSECURITY_DESCRIPTOR pvSD, 
	FUNC_InterpretRights *procItr, void *userctx)
{
	g_dbgbuf[0] = '\0';
	CH10_DumpSD(pvSD, procItr, userctx);

	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
	
	vaAppendText_mled(hedit, _T("==== %s\r\n\r\n"), path);
	// -- Don't overwrite existing messages. Existing error messages can be informational.

	vaAppendText_mled(hedit, _T("%s"), g_dbgbuf);
}

void do_GetSD(HWND hdlg)
{
	HWND hemsg = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
	vaSetWindowText(hemsg, _T("")); // clear text

	DWORD winerr = 0;
	TCHAR path[MAX_PATH] = {};
	GetDlgItemText(hdlg, IDC_EDIT_Path, path, ARRAYSIZE(path));
	if(!path[0])
		return;

	ITR_st itrctx = { FodUnknown };

	DWORD attrs = GetFileAttributes(path);
	if(attrs!=INVALID_FILE_ATTRIBUTES)
	{
		itrctx.fod = (attrs&FILE_ATTRIBUTE_DIRECTORY) ? FodDir : FodFile;
	}
	else
	{
		winerr = GetLastError();
		if(winerr==ERROR_FILE_NOT_FOUND 
			|| winerr==ERROR_PATH_NOT_FOUND
			|| winerr==ERROR_INVALID_NAME) 
		{
			vaAppendText_mled(hemsg, _T("Bad path or path not found. WinErr=%s\r\n"),
				ITCSv(winerr, WinError));
		} 
		else 
		{
			// Note: Even GetFileAttributes() reports ERROR_ACCESS_DENIED,
			// GetNamedSecurityInfo() can still success.

			vaAppendText_mled(hemsg, 
				_T("GetFileAttributes() fail with winerr=%s\r\n"), ITCSv(winerr, WinError));
		}
		
		// ignore the error, just consider `path` a normal file.
		attrs = 0;
		vaAppendText_mled(hemsg, 
			_T("Ignore the error above, go on with GetNamedSecurityInfo().\r\n"));
	}

	bool hasAuditPriv = true; // assume true

	// Enable "SeSecurityPrivilege" anyway, SACL_SECURITY_INFORMATION requires it.
	util_EnableWinPriv(SE_SECURITY_NAME); 

	// Strategy: I'll try GetNamedSecurityInfo() twice. 
	// First try with SACL_SECURITY_INFORMATION. Most user will fail
	// with ERROR_PRIVILEGE_NOT_HELD(1314), bcz SACL requires "SeSecurityPrivilege".
	// If fail so, I do second try, this time without SACL_SECURITY_INFORMATION, 
	// most user should succeed this time.

	SECURITY_DESCRIPTOR *pSD = nullptr;
	SID *psidOwner = nullptr;
	SID *psidGroup = nullptr;
	ACL *pDacl = nullptr, *pSacl = nullptr;
	
	SECURITY_INFORMATION siflags = 
		OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION 
		| DACL_SECURITY_INFORMATION | SACL_SECURITY_INFORMATION 
		| LABEL_SECURITY_INFORMATION ; 

	winerr = GetNamedSecurityInfo(path, SE_FILE_OBJECT, siflags,
		(PSID*)&psidOwner,
		(PSID*)&psidGroup,
		&pDacl,
		&pSacl, // to receive Integrity-level
		(PSECURITY_DESCRIPTOR*)&pSD);
	Cec_LocalFree cec_sd = pSD;

	if(winerr==ERROR_PRIVILEGE_NOT_HELD)
	{
		hasAuditPriv = false;

		// Do second try
		siflags &= ~SACL_SECURITY_INFORMATION;
	
		winerr = GetNamedSecurityInfo(path, SE_FILE_OBJECT, siflags,
			(PSID*)&psidOwner,
			(PSID*)&psidGroup,
			&pDacl,
			&pSacl, // to receive Integrity-level
			(PSECURITY_DESCRIPTOR*)&pSD);
		cec_sd = pSD;

		if(winerr)
		{
			vaAppendText_mled(hemsg,
				_T("[2]GetNamedSecurityInfo() fails with winerr=%s\r\n"), ITCSv(winerr, WinError));
			return;
		}
	}
	else if(winerr)
	{
		vaAppendText_mled(hemsg,
			_T("[1]GetNamedSecurityInfo() fails with winerr=%s\r\n"), ITCSv(winerr, WinError));
		return;
	}

	do_DumpSD(hdlg, path, pSD, InterpretRights, &itrctx);

	if(!hasAuditPriv)
	{
		vaAppendText_mled(hemsg, _T("\r\n")); // blank line
		vaAppendText_mled(hemsg,
			_T("Note: You do not have %s, so Auditing ACEs will not be reported.\r\n"), SE_SECURITY_NAME);
	}
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDC_BTN_GetSD:
	{
		do_GetSD(hdlg);
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

	jul->AnchorControl(0,0, 100,0, IDC_EDIT_Path);
	jul->AnchorControl(100,0, 100,0, IDC_BTN_GetSD);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
	jul->AnchorControl(0,100, 100,100, IDC_LBL_Status);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);

	vaSetWindowText(hdlg, _T("NtfsDumpSD v%d.%d.%d %s"), 
		NtfsDumpSD_VMAJOR, NtfsDumpSD_VMINOR, NtfsDumpSD_VPATCH,
		IsUserAnAdmin() ? _T("(RunAsAdmin)") : _T(""));

	TCHAR textbuf[MAX_PATH] = {};
	GetSystemDirectory(textbuf, ARRAYSIZE(textbuf));
	SetDlgItemText(hdlg, IDC_EDIT_Path, textbuf);

	do_GetSD(hdlg);

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_EDIT_Path));
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
