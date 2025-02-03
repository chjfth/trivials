#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <Aclapi.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

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

struct ITR_st
{
	bool isDir;
};


TCHAR* InterpretRights(DWORD rights, void *userctx)
{
	const int indents = 4;
	const int BufChars = 4000;
	TCHAR *pbuf = new TCHAR[BufChars];
	pbuf[0] = '\0';

#define DUMP_ONE_BIT_PERLINE(bitname) \
	if(rights & (bitname)) \
	_sntprintf_s(pbuf, BufChars, _TRUNCATE, _T("%s%*s (0x%08X) %s\r\n"), pbuf, indents, _T(""), \
	(bitname), _T(#bitname));

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

	if(ctx.isDir)
	{
		DUMP_ONE_BIT_PERLINE(FILE_WRITE_ATTRIBUTES); // 0x100
		DUMP_ONE_BIT_PERLINE(FILE_READ_ATTRIBUTES);  // 0x80
		DUMP_ONE_BIT_PERLINE(FILE_DELETE_CHILD);     // 0x40
		DUMP_ONE_BIT_PERLINE(FILE_TRAVERSE);         // 0x20
		DUMP_ONE_BIT_PERLINE(FILE_WRITE_EA);         // 0x10
		DUMP_ONE_BIT_PERLINE(FILE_READ_EA);          // 0x08
		DUMP_ONE_BIT_PERLINE(FILE_ADD_SUBDIRECTORY); // 0x04
		DUMP_ONE_BIT_PERLINE(FILE_ADD_FILE);         // 0x02
		DUMP_ONE_BIT_PERLINE(FILE_LIST_DIRECTORY);   // 0x01
	}
	else
	{
		DUMP_ONE_BIT_PERLINE(FILE_WRITE_ATTRIBUTES); // 0x100
		DUMP_ONE_BIT_PERLINE(FILE_READ_ATTRIBUTES);  // 0x80
		                                             // 0x40 (none for file)
		DUMP_ONE_BIT_PERLINE(FILE_EXECUTE);          // 0x20
		DUMP_ONE_BIT_PERLINE(FILE_WRITE_EA);         // 0x10
		DUMP_ONE_BIT_PERLINE(FILE_READ_EA);          // 0x08
		DUMP_ONE_BIT_PERLINE(FILE_APPEND_DATA);      // 0x04
		DUMP_ONE_BIT_PERLINE(FILE_WRITE_DATA);       // 0x02
		DUMP_ONE_BIT_PERLINE(FILE_READ_DATA);        // 0x01
	}

	DWORD unknown_bits = rights & ~0xF01F01FF;
	if(unknown_bits)
	{
		_sntprintf_s(pbuf, BufChars, _TRUNCATE, 
			_T("%s%*s Weird: Meet unknown access-right-bits: 0x%08X\r\n"), pbuf, indents, _T(""), 
			unknown_bits);
	}

	return pbuf;
}

void do_DumpSD(HWND hdlg, PSECURITY_DESCRIPTOR pvSD, 
	FUNC_InterpretRights *procItr, void *userctx)
{
	g_dbgbuf[0] = '\0';
	CH10_DumpSD(pvSD, procItr, userctx);

	vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, _T("%s"), g_dbgbuf);
}

void do_GetSD(HWND hdlg)
{
	DWORD winerr = 0;
	TCHAR path[MAX_PATH] = {};
	GetDlgItemText(hdlg, IDC_EDIT_Path, path, ARRAYSIZE(path));
	if(!path[0])
		return;

	DWORD attrs = GetFileAttributes(path);
	if(attrs==INVALID_FILE_ATTRIBUTES)
	{
		winerr = GetLastError();
		if(winerr==ERROR_FILE_NOT_FOUND || winerr==ERROR_PATH_NOT_FOUND) {
			vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, _T("Path not found."));
		} else {
			vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
				_T("GetFileAttributes() fail with winerr=%d"), winerr);
		}
		return;
	}

	SECURITY_DESCRIPTOR *pSD = nullptr;
	SID *psidOwner = nullptr;
	SID *psidGroup = nullptr;
	ACL *pAcl = nullptr;

	winerr = GetNamedSecurityInfo(path, SE_FILE_OBJECT, 
		OWNER_SECURITY_INFORMATION|GROUP_SECURITY_INFORMATION|DACL_SECURITY_INFORMATION,
		(PSID*)&psidOwner,
		(PSID*)&psidGroup,
		&pAcl,
		NULL, // SACL
		(PSECURITY_DESCRIPTOR*)&pSD);
	Cec_LocalFree cec_sd = pSD;

	if(winerr)
	{
		vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
			_T("GetNamedSecurityInfo() fails with winerr=%d"), winerr);
		return;
	}

	ITR_st itrctx = { attrs&FILE_ATTRIBUTE_DIRECTORY ? true : false };
	do_DumpSD(hdlg, pSD, InterpretRights, &itrctx);
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

	vaSetWindowText(hdlg, _T("NtfsDumpSD v%d.%d.%d"), 
		NtfsDumpSD_VMAJOR, NtfsDumpSD_VMINOR, NtfsDumpSD_VPATCH);

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
