#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ShlObj.h>
#include <Winternl.h> // for NtQueryInformationProcess()
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
#include <mswin/dlptr_winapi.h>

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

struct real_PROCESS_BASIC_INFORMATION // told by GPT4o
{
	PVOID Reserved1;
	PVOID PebBaseAddress;
	PVOID Reserved2[2];
	ULONG_PTR UniqueProcessId;
	ULONG_PTR InheritedFromUniqueProcessId; // (undoc) Parent PID
};

DEFINE_DLPTR_WINAPI("ntdll.dll", NtQueryInformationProcess)

DWORD util_GetParentProcessID(DWORD pid) 
{
	if(!dlptr_NtQueryInformationProcess)
		return 0;

	DWORD parentPID = 0;

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
	if (hProcess) {
		real_PROCESS_BASIC_INFORMATION pbi = {};
		ULONG len;
		if (dlptr_NtQueryInformationProcess(hProcess, ProcessBasicInformation, 
			(PROCESS_BASIC_INFORMATION*)&pbi, sizeof(pbi), &len) == 0) 
		{
			parentPID = (DWORD)pbi.InheritedFromUniqueProcessId;
		}
		CloseHandle(hProcess);
	}

	return parentPID;
}

void test_CreateProcessAsUser(HWND hdlg, DWORD onto_pid, TCHAR *exepath)
{
	showmsg(hdlg, _T(""));

	HANDLE hProcess = OpenProcess(PROCESS_CREATE_PROCESS, FALSE, onto_pid);
	CEC_PTRHANDLE cec_hProcess = hProcess;
	if(!hProcess) {
		appendmsg(hdlg, _T("OpenProcess(pid=%d) fail, WinErr=%s"), onto_pid, ITCS_WinError);
		return;
	}

	appendmsg(hdlg, _T("OpenProcess(pid=%d) with PROCESS_CREATE_THREAD, hProcess=0x%X.\r\n"), onto_pid, PtrToUint(hProcess));

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

	// Verify process parent-child relation ship.

	DWORD ppid = util_GetParentProcessID(pi.dwProcessId);
	if(ppid==onto_pid) {
		appendmsg(hdlg, _T("Verify OK. New process's parent is PID %d.\r\n"), onto_pid);
	} else {
		appendmsg(hdlg, _T("PANIC! New process's parent is NOT PID %d, it reports %d.\r\n"), onto_pid, ppid);
	}

	DeleteProcThreadAttributeList(siex.lpAttributeList); // M$ requires this cleaning
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
	
	vaSetWindowText(hdlg, _T("CreateProcessOnto v%d.%d.%d %s"), 
		CreateProcessOnto_VMAJOR, CreateProcessOnto_VMINOR, CreateProcessOnto_VPATCH,
		IsUserAnAdmin()?_T("(RunAsAdmin)"):_T("")
		);

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
