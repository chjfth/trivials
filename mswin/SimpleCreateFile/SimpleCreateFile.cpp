#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <WinIoCtl.h>
#include <ShellAPI.h>
#include <CommCtrl.h>
#include <windowsx.h> // should be after ShellAPI.h to see _INC_SHELLAPI
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"

#include "iversion.h"

#include "utils.h"

#include <mswin/WinError.itc.h>
#include <mswin/winnt.itc.h>
#include <mswin/WinBase.itc.h>
#include <mswin/winuser.itc.h>
using namespace itc;

#include <EnsureClnup_mswin.h>

#define JAUTOBUF_IMPL
#include <JAutoBuf.h>
typedef JAutoBuf<TCHAR, sizeof(TCHAR), 1> AutoTCHARs;

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#define Combobox_EnableWideDrop_IMPL
#include <mswin/Combobox_EnableWideDrop.h>


#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

const int MaxPathBig = 32768;

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
	return _tcstoul(tbuf, nullptr, 0); // could be (-1), if bad string
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
	if(dwDesiredAccess!=-1)
	{
		vaAppendText_mled(hemsg, _T("dwDesiredAccess=0x%X (interpret as %s):\r\n"), 
			dwDesiredAccess, o2i.objtype);
		vaAppendText_mled(hemsg, _T("%s\r\n\r\n"), ITCSv(dwDesiredAccess, *o2i.pitc));
	}
	else
	{	// A value larger than 32bit(eg 0xC00012345) could results.
		vaAppendText_mled(hemsg, _T("dwDesiredAccess (bad value)\r\n\r\n"));
	}

	// dwShareMode

	DWORD dwShareMode = get_EditboxValue(hdlg, IDE_dwShareMode);
	if(dwShareMode!=-1)
	{
		vaAppendText_mled(hemsg, _T("dwShareMode=0x%02X:\r\n"), dwShareMode);
		vaAppendText_mled(hemsg, _T("%s\r\n\r\n"), ITCSv(dwShareMode, itc::dwShareMode));
	}
	else
	{
		vaAppendText_mled(hemsg, _T("dwShareMode (bad value)\r\n\r\n"));
	}

	// dwCreationDisposition

	DWORD dwCreationDisposition = get_EditboxValue(hdlg, IDE_dwCreationDisposition);
	if(dwCreationDisposition!=-1)
	{
		vaAppendText_mled(hemsg, _T("dwCreationDisposition=%u\r\n"), dwCreationDisposition);
		vaAppendText_mled(hemsg, _T("%s\r\n\r\n"), ITCSv(dwCreationDisposition, itc::dwCreationDisposition));
	}
	else
	{
		vaAppendText_mled(hemsg, _T("dwCreationDisposition (bad value)\r\n\r\n"));
	}

	// dwFlagsAndAttributes

	DWORD dwFlagsAndAttributes = get_EditboxValue(hdlg, IDE_dwFlagsAndAttributes);
	if(dwFlagsAndAttributes!=-1)
	{
		vaAppendText_mled(hemsg, _T("dwFlagsAndAttributes=0x%X\r\n"), dwFlagsAndAttributes);
		vaAppendText_mled(hemsg, _T("%s"), ITCSv(dwFlagsAndAttributes, itc::dwFlagsAndAttributes));
	}
	else
	{
		vaAppendText_mled(hemsg, _T("dwFlagsAndAttributes (bad value)\r\n\r\n"));
	}
}

static void enable_DlgItem(HWND hdlg, UINT idUic, bool enable)
{
	HWND huic = GetDlgItem(hdlg, idUic);
	assert(huic);

	EnableWindow(huic, enable);
}

static void btn_EnableCreateFile(HWND hdlg, bool isEnCreateFile)
{
	enable_DlgItem(hdlg, IDB_CreateFile, isEnCreateFile ? true : false);
	enable_DlgItem(hdlg, IDB_CloseHandle, isEnCreateFile ? false : true);

	SetFocus(GetDlgItem(hdlg, isEnCreateFile ? IDB_CreateFile : IDB_CloseHandle));

	util_SetDlgDefaultButton(hdlg, isEnCreateFile ? IDB_CreateFile : IDB_CloseHandle);
}

void do_CreateFile(HWND hdlg)
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	assert(prdata->hFile==NULL);

	HWND helog = GetDlgItem(hdlg, IDE_LogMsg);
	SetWindowText(helog, _T(""));

	TCHAR openpath[MaxPathBig] = _T("");
	GetDlgItemText(hdlg, IDE_lpFileName, openpath, ARRAYSIZE(openpath));

	DWORD dwDesiredAccess = get_EditboxValue(hdlg, IDE_dwDesiredAccess);
	DWORD dwShareMode = get_EditboxValue(hdlg, IDE_dwShareMode);
	DWORD dwCreationDisposition = get_EditboxValue(hdlg, IDE_dwCreationDisposition);
	DWORD dwFlagsAndAttributes = get_EditboxValue(hdlg, IDE_dwFlagsAndAttributes);
	BOOL isMakeSparse = Button_GetCheck(GetDlgItem(hdlg, IDCKB_MakeSparse));

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
		// When CREATE_ALWAYS, CreateFile can succeed with GetLastError()=ERROR_ALREADY_EXISTS .
		vaAppendText_mled(helog, _T("GetLastError()=%s.\r\n"), ITCSv(winerr, WinError));
	}

	if(isMakeSparse)
	{
		DWORD bytesReturned = 0;
		BOOL succ = DeviceIoControl(hfile,
			FSCTL_SET_SPARSE,  // Control code for setting sparse
			NULL, 0, NULL, 0,
			&bytesReturned, NULL);

		if(succ)
			vaAppendText_mled(helog, _T("DeviceIoControl(FSCTL_SET_SPARSE) success.\r\n")); 
		else
			vaAppendText_mled(helog, _T("DeviceIoControl(FSCTL_SET_SPARSE) fail, WinErr=%s\r\n"), ITCS_WinError);
	}

	// Show file attributes
	DWORD attr = GetFileAttributes(openpath);
	if(attr==INVALID_FILE_ATTRIBUTES)
	{
		vaAppendText_mled(helog, _T("GetFileAttributes() error, WinErr=%s"), ITCS_WinError);
	}
	else
	{
		vaAppendText_mled(helog, _T("GetFileAttributes() reports 0x%X :\r\n%s"), 
			attr, ITCSv(attr, FILE_ATTRIBUTE_xxx));
	}

	btn_EnableCreateFile(hdlg, false);
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

	btn_EnableCreateFile(hdlg, true);
}

bool Is_FileExist(const TCHAR *filepath)
{
	DWORD attr = GetFileAttributes(filepath);
	if(attr!=INVALID_FILE_ATTRIBUTES && !(attr&FILE_ATTRIBUTE_DIRECTORY))
		return true;
	else 
		return false;
}

bool do_SaveIni(HWND hdlg, const TCHAR *inipath_input)
{
	TCHAR inipath[MaxPathBig] = _T("");
	GetFullPathName(inipath_input, ARRAYSIZE(inipath), inipath, NULL);

	HANDLE hfile = CreateFile(inipath,
		GENERIC_READ|GENERIC_WRITE,
		0, // dwShareMode
		NULL, // secu-attr
		CREATE_ALWAYS, // dwCreationDisposition
		0, NULL);
	CEC_FILEHANDLE cec_hfile = hfile;
	if(hfile==INVALID_HANDLE_VALUE)
	{
		vaMsgBox(hdlg, MB_OK|MB_ICONERROR, NULL,
			_T("Cannot create file: \n\n%s\n\nWinErr=%s"), inipath, ITCS_WinError);
		return false;
	}

	TCHAR lpFileName[MaxPathBig] = _T("");
	const int ValueLen = 20;
	TCHAR dwDesiredAccess[ValueLen] = {};
	TCHAR dwShareMode[ValueLen] = {};
	TCHAR dwCreationDisposition[ValueLen] = {};
	TCHAR dwFlagsAndAttributes[ValueLen] = {};

	GetDlgItemText(hdlg, IDE_lpFileName, lpFileName, ARRAYSIZE(lpFileName));
	GetDlgItemText(hdlg, IDE_dwDesiredAccess, dwDesiredAccess, ValueLen);
	GetDlgItemText(hdlg, IDE_dwShareMode, dwShareMode, ValueLen);
	GetDlgItemText(hdlg, IDE_dwCreationDisposition, dwCreationDisposition, ValueLen);
	GetDlgItemText(hdlg, IDE_dwFlagsAndAttributes, dwFlagsAndAttributes, ValueLen);
	BOOL isMakeSparse = Button_GetCheck(GetDlgItem(hdlg, IDCKB_MakeSparse));

	TCHAR initext[MaxPathBig+400] = _T("");
	_sntprintf_s(initext, _TRUNCATE, 
		_T("[global]\r\n")
		_T("lpFileName=%s\r\n")
		_T("dwDesiredAccess=%s\r\n")
		_T("dwShareMode=%s\r\n")
		_T("dwCreationDisposition=%s\r\n")
		_T("dwFlagsAndAttributes=%s\r\n")
		_T("isMakeSparse=%d\r\n")
		,
		lpFileName, 
		dwDesiredAccess, dwShareMode, dwCreationDisposition, dwFlagsAndAttributes,
		isMakeSparse
		);

	DWORD bytesToWr = (DWORD) (_tcslen(initext) * sizeof(TCHAR));
	DWORD bytesWritten = 0;

	if(sizeof(TCHAR)==2)
	{	// write extra UTF16LE BOM.
		WriteFile(hfile, "\xFF\xFE", 2, &bytesWritten, NULL);
	}

	BOOL succ = WriteFile(hfile, initext, bytesToWr, &bytesWritten, NULL);
	if(!succ)
	{
		vaMsgBox(hdlg, MB_OK|MB_ICONERROR, NULL,
			_T("WriteFile(\"%s\") fail. WinErr=%s"), inipath, ITCS_WinError);

		return false;
	}

	vaSetDlgItemText(hdlg, IDE_LogMsg, _T("INI file saved:\r\n%s"), inipath);
	return true;
}

bool Is_StringInCombobox(HWND hcbx, const TCHAR *item)
{
	AutoTCHARs textbuf = 0;
	int items = ComboBox_GetCount(hcbx);
	for(int i=0; i<items; i++)
	{
		int textlen = ComboBox_GetLBTextLen(hcbx, i);
		if(textlen+1 > (int)textbuf.Size())
			textbuf = textlen+1;

		ComboBox_GetLBText(hcbx, i, textbuf);

		if(_tcsicmp(textbuf, item)==0)
			return true;
	}

	return false;
}

void do_SaveIni_ButtonClick(HWND hdlg)
{
	SetDlgItemText(hdlg, IDE_LogMsg, _T(""));

	HWND hcbx = GetDlgItem(hdlg, IDCB_IniList);
	TCHAR inipath[MAX_PATH] = {};
	ComboBox_GetText(hcbx, inipath, ARRAYSIZE(inipath));

	if(!inipath[0])
		return;

	if(Is_FileExist(inipath))
	{
		int ret = vaMsgBox(hdlg, MB_YESNO|MB_ICONQUESTION, NULL, 
			_T("Overwrite \"%s\" ?"), inipath);
		
		if(ret==IDNO)
			return;
	}

	if( do_SaveIni(hdlg, inipath) )
	{	// Add inipath to Combobox list.
		if(! Is_StringInCombobox(hcbx, inipath))
			ComboBox_AddString(hcbx, inipath);
	}
}

void do_LoadIni(HWND hdlg, const TCHAR *inipath_input)
{
	// Note: If inipath is a filename, it is relative to C:\Windows ,
	// so we need to get its fullpath first.

	TCHAR inipath[MaxPathBig] = _T("");
	GetFullPathName(inipath_input, ARRAYSIZE(inipath), inipath, NULL);

	TCHAR tbuf[MaxPathBig] = _T("");
	DWORD cret = 0;
	
	tbuf[0] = '\0';
	cret = GetPrivateProfileString(_T("global"), _T("lpFileName"), NULL, tbuf, ARRAYSIZE(tbuf), inipath);
	SetDlgItemText(hdlg, IDE_lpFileName, tbuf);
	
	tbuf[0] = '\0';
	cret = GetPrivateProfileString(_T("global"), _T("dwDesiredAccess"), NULL, tbuf, ARRAYSIZE(tbuf), inipath);
	SetDlgItemText(hdlg, IDE_dwDesiredAccess, tbuf);

	tbuf[0] = '\0';
	cret = GetPrivateProfileString(_T("global"), _T("dwShareMode"), NULL, tbuf, ARRAYSIZE(tbuf), inipath);
	SetDlgItemText(hdlg, IDE_dwShareMode, tbuf);

	tbuf[0] = '\0';
	cret = GetPrivateProfileString(_T("global"), _T("dwCreationDisposition"), NULL, tbuf, ARRAYSIZE(tbuf), inipath);
	SetDlgItemText(hdlg, IDE_dwCreationDisposition, tbuf);

	tbuf[0] = '\0';
	cret = GetPrivateProfileString(_T("global"), _T("dwFlagsAndAttributes"), NULL, tbuf, ARRAYSIZE(tbuf), inipath);
	SetDlgItemText(hdlg, IDE_dwFlagsAndAttributes, tbuf);

	cret = GetPrivateProfileString(_T("global"), _T("isMakeSparse"), _T("0"), tbuf, ARRAYSIZE(tbuf), inipath);
	Button_SetCheck(GetDlgItem(hdlg, IDCKB_MakeSparse), tbuf[0]=='1'?TRUE:FALSE); 

	vaSetDlgItemText(hdlg, IDE_LogMsg, _T("INI file loaded:\r\n%s"), inipath);
}

void do_LoadIni_ButtonClick(HWND hdlg)
{
	SetDlgItemText(hdlg, IDE_LogMsg, _T(""));

	HWND hcbx = GetDlgItem(hdlg, IDCB_IniList);
	TCHAR inipath[MAX_PATH] = {};
	ComboBox_GetText(hcbx, inipath, ARRAYSIZE(inipath));

	if(!inipath[0])
		return;

	if(!Is_FileExist(inipath))
	{
		vaMsgBox(hdlg, MB_OK|MB_ICONERROR, NULL,
			_T("INI file not exist: \"%s\""), inipath);
		return;
	}

	do_LoadIni(hdlg, inipath);
}

void Dlg_OnDropFiles(HWND hdlg, HDROP hdrop)
{
	TCHAR inipath[MAX_PATH] = {};
	UINT pathlen = DragQueryFile(hdrop, 0, inipath, ARRAYSIZE(inipath));
	DragFinish(hdrop);

	if(_tcsicmp(inipath+pathlen-4, _T(".ini"))!=0)
	{
		vaMsgBox(hdlg, MB_OK|MB_ICONWARNING, NULL,
			_T("The dropped file is not an INI file. I'll do nothing."));
		return;
	}

	HWND hcbx = GetDlgItem(hdlg, IDCB_IniList);

	if(! Is_StringInCombobox(hcbx, inipath))
		ComboBox_AddString(hcbx, inipath);

	ComboBox_SetText(hcbx, inipath);
	do_LoadIni_ButtonClick(hdlg);
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

	case IDB_Load:
		do_LoadIni_ButtonClick(hdlg);
		break;

	case IDB_Save:
		do_SaveIni_ButtonClick(hdlg);
		break;

	case IDOK:
	case IDCANCEL:
		{
			EndDialog(hdlg, idCtrl);
			break;
		}
	}}
}

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	const int ysplit = 82;
	jul->AnchorControl(0,0, 100,0, IDE_lpFileName);
	jul->AnchorControl(0,0, 100,ysplit, IDE_Interpret);
	jul->AnchorControl(0,ysplit, 100,100, IDE_LogMsg);
	
	jul->AnchorControl(0,100, 0,100, IDB_CreateFile);
	jul->AnchorControl(0,100, 0,100, IDB_CloseHandle);
	
	jul->AnchorControl(0,100, 100,100, IDCB_IniList);
	jul->AnchorControl(100,100, 100,100, IDB_Load);
	jul->AnchorControl(100,100, 100,100, IDB_Save);
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	vaSetWindowText(hdlg, _T("SimpleCreateFile v%d.%d.%d (%s)"), 
		SimpleCreateFile_VMAJOR, SimpleCreateFile_VMINOR, SimpleCreateFile_VPATCH,
		str_ANSIorUnicode());
	
	Dlg_EnableJULayout(hdlg);

	Cf_SetDefaultParams(hdlg);
	Cf_InterpretParams(hdlg);

	btn_EnableCreateFile(hdlg, true);

	HWND hcbx = GetDlgItem(hdlg, IDCB_IniList);
//	ComboBox_SetCueBannerText(hcbx, _T("Drop INI file here to load."));
	SetDlgItemText(hdlg, IDE_LogMsg, 
		_T("Hint: You can save CreateFile parameters to a INI file then load it later.\r\n")
		_T("You can load an INI file by dragging it here.")
		);

	TCHAR szDefaultIni[MAX_PATH] = {};
	_sntprintf_s(szDefaultIni, _TRUNCATE, _T("%s.ini"), GetExeStemname());
	ComboBox_AddString(hcbx, szDefaultIni);
	ComboBox_SetText(hcbx, szDefaultIni);

	Dlgbox_EnableComboboxWideDrop(hdlg);

	DragAcceptFiles(hdlg, TRUE);

	SetFocus(GetDlgItem(hdlg, IDB_CreateFile));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR WINAPI UserDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
		HANDLE_dlgMSG(hdlg, WM_DROPFILES,     Dlg_OnDropFiles);
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
