#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#include <mswin/WinError.itc.h>
#include <mswin/winnt.itc.h>
#include <mswin/WinBase.itc.h>
using namespace itc;

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

const int MaxPath = 32768;

struct DlgPrivate_st
{
	bool isInitialized;
	HANDLE hFile;
};

struct ObjtypeToItc_st
{
	const TCHAR *objtype;
	const CInterpretConst *pitc;
};

ObjtypeToItc_st gar_objtype2itc[] =
{
	{ _T("File"), &FileRights },
	{ _T("Directory"), &DirectoryRights },
};


void Cf_SetDefaultParams(HWND hdlg)
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	TCHAR tbuf[20] = _T("");
	
	SetDlgItemText(hdlg, IDE_lpFileName, _T("NewFile1.txt"));
	
	_sntprintf_s(tbuf, _TRUNCATE, _T("0x%X"), GENERIC_READ|GENERIC_WRITE);
	SetDlgItemText(hdlg, IDE_dwDesiredAccess, tbuf);

	_sntprintf_s(tbuf, _TRUNCATE, _T("0x%X"), FILE_SHARE_READ|FILE_SHARE_WRITE);
	SetDlgItemText(hdlg, IDE_dwShareMode, tbuf);

	_sntprintf_s(tbuf, _TRUNCATE, _T("%u"), CREATE_ALWAYS);
	SetDlgItemText(hdlg, IDE_dwCreationDisposition, tbuf);

	_sntprintf_s(tbuf, _TRUNCATE, _T("0x%X"), 0);
	SetDlgItemText(hdlg, IDE_dwFlagsAndAttributes, tbuf);

	//// 
	
	HWND hcbx = GetDlgItem(hdlg, IDCB_ObjectType);
	for(int i=0; i<ARRAYSIZE(gar_objtype2itc); i++)
	{
		ObjtypeToItc_st &o2i = gar_objtype2itc[i];
		int idx = ComboBox_AddString(hcbx, o2i.objtype);
		ComboBox_SetItemData(hcbx, idx, &o2i);
	}

	ComboBox_SetCurSel(hcbx, 0);

	prdata->isInitialized = true;
}

DWORD get_EditboxValue(HWND hdlg, UINT idedit)
{
	TCHAR tbuf[20] = {};
	GetDlgItemText(hdlg, idedit, tbuf, ARRAYSIZE(tbuf));
	return _tcstoul(tbuf, nullptr, 0);
}

void Cf_InterpretParams(HWND hdlg)
{
	HWND hcbx = GetDlgItem(hdlg, IDCB_ObjectType);
	int idx = ComboBox_GetCurSel(hcbx);
	ObjtypeToItc_st &o2i = *(ObjtypeToItc_st*)ComboBox_GetItemData(hcbx, idx);

	HWND hemsg = GetDlgItem(hdlg, IDE_Interpret);
	vaSetWindowText(hemsg, _T(""));

	// dwDesiredAccess

	DWORD dwDesiredAccess = get_EditboxValue(hdlg, IDE_dwDesiredAccess);
	vaAppendText_mled(hemsg, _T("dwDesiredAccess=0x%X (interpret as %s):\r\n"), 
		dwDesiredAccess, o2i.objtype);

	vaAppendText_mled(hemsg, _T("%s\r\n\r\n"), ITCSv(dwDesiredAccess, *o2i.pitc));

	// dwShareMode

	DWORD dwShareMode = get_EditboxValue(hdlg, IDE_dwShareMode);
	vaAppendText_mled(hemsg, _T("dwShareMode=0x%02X):\r\n"), dwShareMode);
	vaAppendText_mled(hemsg, _T("%s\r\n\r\n"), ITCSv(dwShareMode, itc::dwShareMode));

	// dwCreationDisposition

	DWORD dwCreationDisposition = get_EditboxValue(hdlg, IDE_dwCreationDisposition);
	vaAppendText_mled(hemsg, _T("dwCreationDisposition=%u\r\n"), dwCreationDisposition);
	vaAppendText_mled(hemsg, _T("%s\r\n\r\n"), ITCSv(dwCreationDisposition, itc::dwCreationDisposition));

	// dwFlagsAndAttributes

	DWORD dwFlagsAndAttributes = get_EditboxValue(hdlg, IDE_dwFlagsAndAttributes);
	vaAppendText_mled(hemsg, _T("dwFlagsAndAttributes=0x%X\r\n"), dwFlagsAndAttributes);
	vaAppendText_mled(hemsg, _T("%s\r\n\r\n"), ITCSv(dwFlagsAndAttributes, itc::dwFlagsAndAttributes));
}

static void enable_DlgItem(HWND hdlg, UINT idUic, bool enable)
{
	HWND huic = GetDlgItem(hdlg, idUic);
	assert(huic);

	EnableWindow(huic, enable);
}

void do_CreateFile(HWND hdlg)
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	assert(prdata->hFile==NULL);

	HWND helog = GetDlgItem(hdlg, IDE_LogMsg);
	SetWindowText(helog, _T(""));

	TCHAR openpath[MaxPath] = _T("");
	GetDlgItemText(hdlg, IDE_lpFileName, openpath, ARRAYSIZE(openpath));

	DWORD dwDesiredAccess = get_EditboxValue(hdlg, IDE_dwDesiredAccess);
	DWORD dwShareMode = get_EditboxValue(hdlg, IDE_dwShareMode);
	DWORD dwCreationDisposition = get_EditboxValue(hdlg, IDE_dwCreationDisposition);
	DWORD dwFlagsAndAttributes = get_EditboxValue(hdlg, IDE_dwFlagsAndAttributes);

	SetLastError(0); 

	HANDLE hfile = CreateFile(openpath, dwDesiredAccess, dwShareMode, 
		NULL, // security-attributes
		dwCreationDisposition,
		dwFlagsAndAttributes,
		NULL);
	DWORD winerr = GetLastError();
	if(hfile==INVALID_HANDLE_VALUE)
	{
		vaAppendText_mled(helog, _T("CreateFile() fail, WinErr=%s"), ITCSv(winerr, WinError));
		return;
	}

	prdata->hFile = hfile;

	vaAppendText_mled(helog, _T("CreateFile() success. Handle=0x%X.\r\n"), hfile);
	if(winerr)
	{
		// When CREAT_ALWAYS, CreateFile can succeed with GetLastError()=ERROR_ALREADY_EXISTS .
		vaAppendText_mled(helog, _T("GetLastError()=%s.\r\n"), ITCSv(winerr, WinError));
	}

	enable_DlgItem(hdlg, IDB_CreateFile, false);
	enable_DlgItem(hdlg, IDB_CloseHandle, true);
}

void do_CloseFile(HWND hdlg)
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	assert(prdata->hFile!=NULL);

	HWND helog = GetDlgItem(hdlg, IDE_LogMsg);

	BOOL Succ = CloseHandle(prdata->hFile);
	if(Succ)
		vaSetWindowText(helog, _T("CloseHandle(0x%X) success."), prdata->hFile);
	else {
		vaSetWindowText(helog, _T("Unexpect! CloseHandle(0x%X) fail, WinErr=%s"),
			prdata->hFile, ITCS_WinError);

		return;
	}

	prdata->hFile = NULL;

	enable_DlgItem(hdlg, IDB_CreateFile, true);
	enable_DlgItem(hdlg, IDB_CloseHandle, false);
}

void Dlg_OnCommand(HWND hdlg, int idCtrl, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	if(codeNotify==EN_CHANGE || codeNotify==CBN_SELCHANGE)
	{
		if(prdata->isInitialized && idCtrl!=IDE_Interpret && idCtrl!=IDE_LogMsg)
		{
			Cf_InterpretParams(hdlg);
		}

		return;
	}

	//////////////////////

	switch (idCtrl) 
	{{
	case IDB_CreateFile:
		do_CreateFile(hdlg);
		break;

	case IDB_CloseHandle:
		do_CloseFile(hdlg);
		break;

	case IDOK:
	case IDCANCEL:
		{
			EndDialog(hdlg, idCtrl);
			break;
		}
	}}
}

BOOL Dlg_OnNotify(HWND hdlg, int idCtrl, LPNMHDR pnmhdr) 
{
	UINT noticode = pnmhdr->code;

	if(noticode==EN_CHANGE || noticode==CBN_SELCHANGE) // wrong using EN_CHANGE
	{
		if(idCtrl!=IDE_LogMsg)
		{
			Cf_InterpretParams(hdlg);
		}
	}

	return 0;
}


static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDE_lpFileName);
	jul->AnchorControl(0,0, 100,100, IDE_Interpret);
	jul->AnchorControl(0,100, 100,100, IDE_LogMsg);
	jul->AnchorControl(0,100, 0,100, IDB_CreateFile);
	jul->AnchorControl(0,100, 0,100, IDB_CloseHandle);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	vaSetWindowText(hdlg, _T("SimpleCreateFile v%d.%d.%d"), 
		SimpleCreateFile_VMAJOR, SimpleCreateFile_VMINOR, SimpleCreateFile_VPATCH);
	
	Dlg_EnableJULayout(hdlg);

	Cf_SetDefaultParams(hdlg);
	Cf_InterpretParams(hdlg);

	enable_DlgItem(hdlg, IDB_CreateFile, true);
	enable_DlgItem(hdlg, IDB_CloseHandle, false);

	SetFocus(GetDlgItem(hdlg, IDB_CreateFile));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR WINAPI UserDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
//		HANDLE_dlgMSG(hdlg, WM_NOTIFY,        Dlg_OnNotify);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	DlgPrivate_st dlgdata = { };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, (LPARAM)&dlgdata);

	return 0;
}
