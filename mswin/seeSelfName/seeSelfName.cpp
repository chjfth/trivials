#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <psapi.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"
#include "iversion.h"

#include "../utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *pszCmdLine;
};

const TCHAR *GatherInfo(const TCHAR *pszWinMainCmdline)
{
	static TCHAR s_info[8000] = {};

	TCHAR obuf[MAX_PATH+1] = {};

	GetModuleFileName(NULL, obuf, MAX_PATH);
	//
	_sntprintf_s(s_info, _TRUNCATE, 
		_T("%s")
		_T("GetModuleFileName(): %d chars\r\n")
		_T("%s\r\n\r\n")
		, s_info, _tcslen(obuf), obuf);

	HANDLE hSelfProcess = GetCurrentProcess();
	GetModuleFileNameEx(hSelfProcess, NULL, obuf, MAX_PATH);
	//
	_sntprintf_s(s_info, _TRUNCATE,
		_T("%s")
		_T("GetModuleFileNameEx(): %d chars\r\n")
		_T("%s\r\n\r\n")
		, s_info, _tcslen(obuf), obuf);

	GetProcessImageFileName(GetCurrentProcess(), obuf, MAX_PATH);
	//
	_sntprintf_s(s_info, _TRUNCATE, 
		_T("%s")
		_T("GetProcessImageFileName(): %d chars\r\n")
		_T("%s\r\n\r\n")
		, s_info, _tcslen(obuf), obuf);

	DWORD retlen = MAX_PATH;
	QueryFullProcessImageName(GetCurrentProcess(), 0, obuf, &retlen);
	//
	_sntprintf_s(s_info, _TRUNCATE, 
		_T("%s")
		_T("QueryFullProcessImageName(0): %d chars\r\n")
		_T("%s\r\n\r\n")
		, s_info, _tcslen(obuf), obuf);

	retlen = MAX_PATH;
	QueryFullProcessImageName(GetCurrentProcess(), PROCESS_NAME_NATIVE, obuf, &retlen);
	//
	_sntprintf_s(s_info, _TRUNCATE, 
		_T("%s")
		_T("QueryFullProcessImageName(PROCESS_NAME_NATIVE): %d chars\r\n")
		_T("%s\r\n\r\n")
		, s_info, _tcslen(obuf), obuf);

	const TCHAR * pszWholeCmdLine = GetCommandLine();
	//
	_sntprintf_s(s_info, _TRUNCATE, 
		_T("%s")
		_T("GetCommandLine(): %d chars\r\n")
		_T("%s\r\n\r\n")
		, s_info, _tcslen(pszWholeCmdLine), pszWholeCmdLine);

	_sntprintf_s(s_info, _TRUNCATE, 
		_T("%s")
		_T("WinMain()'s lpCmdLine: %d chars\r\n")
		_T("%s\r\n\r\n")
		, s_info, _tcslen(pszWinMainCmdline), pszWinMainCmdline);

	return s_info;
}



BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		seeSelfName_VMAJOR, seeSelfName_VMINOR, seeSelfName_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);

	const TCHAR *info = GatherInfo(prdata->pszCmdLine);
	SetDlgItemText(hdlg, IDC_EDIT1, info);

	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT1);
	jul->AnchorControl(50,100, 50,100, IDC_BUTTON1);

	SetFocus(NULL);
	return 0; // Let Dlg-manager respect SetFocus()? no use!
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	switch (id) 
	{{
	case IDOK:
	case IDCANCEL:
		{
			EndDialog(hdlg, id);
			break;
		}
	}}
}

INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) 
{
	g_hinstExe = hinstExe;

	DlgPrivate_st dlgdata = { pszCmdLine };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)&dlgdata);

	return 0;
}
