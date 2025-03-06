#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ShlObj.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#include <EnsureClnup_mswin.h>
#include <mswin/WinError.itc.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

static void showmsg(HWND hdlg, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);
	vlSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, szfmt, args);
	va_end(args);
}

static void appendmsg(HWND hdlg, const TCHAR *szfmt, ...)
{
	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
	va_list args;
	va_start(args, szfmt);
	vlAppendText_mled(hedit, szfmt, args);
	va_end(args);
}

//MakeDelega_CleanupPtr_winapi(Cec_DeleteProcThreadAttributeList, void, DeleteProcThreadAttributeList, LPPROC_THREAD_ATTRIBUTE_LIST)

void test_CreateProcessAsUser(HWND hdlg, DWORD pid, TCHAR *exepath)
{
	showmsg(hdlg, _T(""));

	HANDLE hProcess = OpenProcess(PROCESS_CREATE_PROCESS, FALSE, pid);
	CEC_PTRHANDLE cec_hProcess = hProcess;
	if(!hProcess) {
		appendmsg(hdlg, _T("OpenProcess(pid=%d) fail, WinErr=%s"), pid, ITCS_WinError);
		return;
	}

	appendmsg(hdlg, _T("OpenProcess(pid=%d) with PROCESS_CREATE_THREAD, hProcess=0x%X.\r\n"), pid, PtrToUint(hProcess));

	SIZE_T dsize = 0;
	InitializeProcThreadAttributeList(NULL, 1, 0, &dsize); // get sizex
	appendmsg(hdlg, _T("One PROC_THREAD_ATTRIBUTE_LIST is %d bytes.\r\n"), int(dsize));

	STARTUPINFOEX siex = {{ sizeof(siex) }};
	char *dsize_buf = new char[dsize];
	CEC_raw_delete cec_dsize = dsize_buf;
	siex.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)dsize_buf;

	BOOL succ = InitializeProcThreadAttributeList(siex.lpAttributeList, 1, 0, &dsize);
	if(!succ) {
		appendmsg(hdlg, _T("InitializeProcThreadAttributeList() fail, WinErr=%s"), ITCS_WinError);
		return;
	}

	succ = UpdateProcThreadAttribute(siex.lpAttributeList, 0, 
		PROC_THREAD_ATTRIBUTE_PARENT_PROCESS,
		&hProcess, sizeof(hProcess),
		NULL, NULL);
	if(!succ) {
		appendmsg(hdlg, _T("UpdateProcThreadAttribute() fail, WinErr=%s"), ITCS_WinError);
		return;
	}

	PROCESS_INFORMATION pi = {};
	succ =  CreateProcess(NULL, 
		exepath, 
		NULL, // new process security-attributes
		NULL, // new thread security-attributes
		FALSE, //  bInheritHandles
		EXTENDED_STARTUPINFO_PRESENT, // dwCreationFlags,
		NULL, // lpEnvironment,
		NULL, // lpCurrentDirectory,
		&siex.StartupInfo,
		&pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if(succ) {
		appendmsg(hdlg, _T("CreateProcess() success, new PID=%d.\r\n"), pi.dwProcessId);
	} else {
		appendmsg(hdlg, _T("CreateProcess() fail, WinErr=%s"), ITCS_WinError);
	}

	DeleteProcThreadAttributeList(siex.lpAttributeList); // M$ requires this cleaning

	//	OpenProcessToken();
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		TCHAR exepath[MAX_PATH] = {};
		DWORD pid = 0; // 0 for self
		BOOL isok = 0;
		pid = GetDlgItemInt(hdlg, IDE_ProcessID, &isok, FALSE);
		GetDlgItemText(hdlg, IDE_ExePath, exepath, MAX_PATH);
		test_CreateProcessAsUser(hdlg, pid, exepath);
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

	jul->AnchorControl(0,0, 100,0, IDE_ExePath);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
	jul->AnchorControl(50,100, 50,100, IDC_BUTTON1);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
//	TCHAR textbuf[200];
//	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
//		CreateProcessOnto_VMAJOR, CreateProcessOnto_VMINOR, CreateProcessOnto_VPATCH);
	
//	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, prdata->mystr);

	vaSetWindowText(hdlg, _T("CreateProcessOnto %s"), IsUserAnAdmin()?_T("(RunAsAdmin)"):_T(""));

	showmsg(hdlg, 
		_T("This program creates a child-process(C) onto an existing parent process(B).\r\n")
		_T("\r\n")
		_T("This feature is available on Windows Vista+.\r\n")
		);

	DWORD pidself = GetCurrentProcessId();
	SetDlgItemInt(hdlg, IDE_ProcessID, pidself, TRUE);
	SetDlgItemText(hdlg, IDE_ExePath, _T("C:\\Windows\\System32\\notepad.exe"));

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
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
