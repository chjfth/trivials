#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <ShlObj.h>
#include <CommCtrl.h>
#include <winternl.h> // for NtQueryObject
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#define JAUTOBUF_IMPL
#include <JAutoBuf.h>

#include <mswin/dlptr_winapi.h>

#include <EnsureClnup_mswin.h>

#include <mswin/WinError.itc.h>
#include <mswin/winnt.itc.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

inline bool Is_LessBuffer(DWORD winerr)
{
	if( winerr==ERROR_INSUFFICIENT_BUFFER ||
		winerr==ERROR_MORE_DATA ||
		winerr==ERROR_BUFFER_OVERFLOW)
		return true;
	else
		return false;
}

static void showmsg(HWND hdlg, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);
	vlSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, szfmt, args);
	va_end(args);
}

static void appendmsg(HWND hdlg, const TCHAR *szfmt=nullptr, ...)
{
	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	if(szfmt)
	{
		va_list args;
		va_start(args, szfmt);
		vlAppendText_mled(hedit, szfmt, args);
		va_end(args);
	}

	vaAppendText_mled(hedit, _T("\r\n"));
}

//MakeDelega_CleanupPtr_winapi(Cec_DeleteProcThreadAttributeList, void, DeleteProcThreadAttributeList, LPPROC_THREAD_ATTRIBUTE_LIST)


DEFINE_DLPTR_WINAPI("ntdll.dll", NtQueryObject)

DWORD get_AccessMaskGrantedToMe(HANDLE hkobj)
{
	// provided by GPT4o.
	// Given a kernel object handle(hkobj), return the access bits that is granted
	// to the calling process.

	PUBLIC_OBJECT_BASIC_INFORMATION obi = {};
	DWORD retlen = 0;
	NTSTATUS ntserr = dlptr_NtQueryObject(hkobj, ObjectBasicInformation, &obi, sizeof(obi), &retlen);
	if(!ntserr)
		return obi.GrantedAccess;
	else
		return -1;
}

bool list_TokenPrivileges(HWND hdlg, HANDLE hToken)
{
	BOOL succ = 0;
	DWORD winerr = 0;
	Jautobuf jbTokenPrivs;
	do {
		succ = GetTokenInformation(hToken, TokenPrivileges, 
			jbTokenPrivs, jbTokenPrivs, jbTokenPrivs);
		winerr = GetLastError();
	} while(!succ && Is_LessBuffer(winerr));

	if(!succ) {
		appendmsg(hdlg, _T("GetTokenInformation() fails, WinErr=%s."), ITCS_WinError);
		return false;
	}

	PTOKEN_PRIVILEGES ptp = (PTOKEN_PRIVILEGES)jbTokenPrivs.Bufptr();

	int privcount = ptp->PrivilegeCount;

	appendmsg(hdlg, _T("Token has %d privileges:"), privcount);

	for(int i=0; i<privcount; i++) 
	{
		LUID luid = ptp->Privileges[i].Luid;
		TCHAR szname[200] = {};
		DWORD ccname = ARRAYSIZE(szname);
		LookupPrivilegeName(NULL, &luid, szname, &ccname);

		appendmsg(hdlg, _T("  [#%d] Luid=%d , %s (%s)"), 
			i+1,
			luid.LowPart, 
			szname,
			(ptp->Privileges[i].Attributes & SE_PRIVILEGE_ENABLED) ? _T("Enabled") : _T("-")
			);
	}

	appendmsg(hdlg); // new line
	return true;
}

bool dump_ProcessTokenInfo(HWND hdlg, HANDLE hProcess)
{
	HANDLE hToken = NULL;
	DWORD reqAccess = TOKEN_QUERY;
	BOOL Succ = OpenProcessToken(hProcess, reqAccess, &hToken);
	CEC_PTRHANDLE cec_hToken = hToken;
	if(Succ) {
//		appendmsg(hdlg, _T("OpenProcessToken() success. hToken=0x%X."), PtrToUint(hToken));
	} else {
		appendmsg(hdlg, _T("OpenProcessToken() fail, WinErr=%s."), ITCS_WinError);
		return false;
	}

	bool succ = list_TokenPrivileges(hdlg, hToken);
	if(!succ)
		return false; 

	DWORD grantedAccess = get_AccessMaskGrantedToMe(hToken);
	assert(grantedAccess==reqAccess);
	return true;
}



void test_token_MAXIMUM_ALLOWED(HWND hdlg, HANDLE hMyProcess, HANDLE hCarrierProcess)
{
	HANDLE hMyToken = NULL, hCarrierToken = NULL;
	BOOL Succ = 0;
	
	Succ = OpenProcessToken(hMyProcess, MAXIMUM_ALLOWED, &hMyToken);
	CEC_PTRHANDLE cec_hMyToken = hMyToken;
	if(Succ) {
		DWORD accessToMyToken = get_AccessMaskGrantedToMe(hMyToken);
		appendmsg(hdlg, 
			_T("MAXIMUM_ALLOWED access bits to my-token: 0x%08X, %s"),
			accessToMyToken,
			ITCSv(accessToMyToken, itc::TokenRights)
			);
	}
	else {
		appendmsg(hdlg, _T("Unexpect! OpenProcessToken(hMyProcess, MAXIMUM_ALLOWED, ...) failed with winerr=%s"), ITCS_WinError);
	}

	Succ = OpenProcessToken(hCarrierProcess, MAXIMUM_ALLOWED, &hCarrierToken);
	CEC_PTRHANDLE cec_hCarrierToken = hCarrierToken;
	if(Succ) {
		DWORD accessToCarrierToken = get_AccessMaskGrantedToMe(hCarrierToken);
		appendmsg(hdlg,
			_T("MAXIMUM_ALLOWED access bits to carrier-token: 0x%08X, %s"),
			accessToCarrierToken,
			ITCSv(accessToCarrierToken, itc::TokenRights)
			);
	}
	else {
		appendmsg(hdlg, _T("Unexpect! OpenProcessToken(hCarrierProcess, MAXIMUM_ALLOWED, ...) failed with winerr=%s"), ITCS_WinError);
	}
}

#if _WIN32_WINNT >= 0x0A00
// Win10+ SDK is available, do nothing here.
#else
// Using an older WinSDK, eg Win7 SDK(VC2010), so we need to declare CompareObjectHandles
BOOL WINAPI CompareObjectHandles(HANDLE hFirstObjectHandle,	HANDLE hSecondObjectHandle);
#endif
DEFINE_DLPTR_WINAPI("KernelBase.dll", CompareObjectHandles) // Win10+

void test_CreateProcessAsUser(HWND hdlg, DWORD pidCarrier, TCHAR *exepath)
{
	DWORD pidMy = GetCurrentProcessId();

	appendmsg(hdlg, _T("====My PID is %d, dumping my process Token privileges===="), pidMy);
	dump_ProcessTokenInfo(hdlg, GetCurrentProcess());

	// Grab a handle of the carrier process.

	HANDLE hProcessCarrier = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pidCarrier);
	CEC_PTRHANDLE cec_hProcess = hProcessCarrier;
	if(hProcessCarrier) 	{
		appendmsg(hdlg, _T("OpenProcess(pid=%d) with PROCESS_QUERY_INFORMATION, hProcess=0x%X."), 
			pidCarrier, PtrToUint(hProcessCarrier));
	}
	else {
		appendmsg(hdlg, _T("OpenProcess() on carrier-PID fail, WinErr=%s."), ITCS_WinError);
		return;
	}

	appendmsg(hdlg); // new line

	appendmsg(hdlg, _T("====Carrier PID is %d, dumping carrier process Token privileges===="), pidCarrier);
	if(pidCarrier==pidMy)
	{
		appendmsg(hdlg, _T("(Omit, same as my PID)"));
		appendmsg(hdlg, _T(""));
	}
	else
	{
		bool succ = dump_ProcessTokenInfo(hdlg, hProcessCarrier);
		if(!succ)
			return;
	}


	DWORD reqAccess = TOKEN_QUERY | TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY;

	HANDLE hTokenCarrier = NULL;
	BOOL Succ = OpenProcessToken(hProcessCarrier, reqAccess, &hTokenCarrier);
	CEC_PTRHANDLE cec_hTokenCarrier = hTokenCarrier;
	if(Succ) {
		appendmsg(hdlg, _T("OpenProcessToken() carrier success. hToken=0x%X."), PtrToUint(hTokenCarrier));
	} else {
		appendmsg(hdlg, _T("OpenProcessToken() carrier fail, WinErr=%s."), ITCS_WinError);
		return;
	}

	DWORD grantedAccess = get_AccessMaskGrantedToMe(hTokenCarrier);
	assert(grantedAccess==reqAccess);

#if 1 // Debugging purpose:
	HANDLE hTokenMy = NULL;
	OpenProcessToken(GetCurrentProcess(), reqAccess, &hTokenMy);
	CEC_PTRHANDLE cec_hTokenMy = hTokenMy;

	if(dlptr_CompareObjectHandles) 
	{
		if(dlptr_CompareObjectHandles(hTokenMy, hTokenCarrier))
			appendmsg(hdlg, _T("My-token and Carrier-token are the same."));
		else 
			appendmsg(hdlg, _T("My-token and Carrier-token are different."));
	}

	test_token_MAXIMUM_ALLOWED(hdlg, GetCurrentProcess(), hProcessCarrier);
#endif

	appendmsg(hdlg); // new line

	STARTUPINFO si = {sizeof(si)};
	PROCESS_INFORMATION pi = {};
	Succ =  CreateProcessAsUser(hTokenCarrier,
		NULL, exepath, 
		NULL, // new process security-attributes
		NULL, // new thread security-attributes
		FALSE, // bInheritHandles
		0,    // dwCreationFlags,
		NULL, // lpEnvironment,
		NULL, // lpCurrentDirectory,
		&si,
		&pi);
	if(Succ) {
		appendmsg(hdlg, _T("CreateProcessAsUser() success, new PID=%d."), pi.dwProcessId);
	} else {
		appendmsg(hdlg, _T("CreateProcessAsUser() fail, WinErr=%s."), ITCS_WinError);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
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
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	vaSetWindowText(hdlg, _T("tCreateProcessAsUser1 v%d.%d.%d %s"), 
		CreateProcessAsUser1_VMAJOR, CreateProcessAsUser1_VMINOR, CreateProcessAsUser1_VPATCH,
		IsUserAnAdmin()?_T("(RunAsAdmin)"):_T("")
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
