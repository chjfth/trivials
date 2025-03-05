#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#include <mswin/WinError.itc.h>

#include <EnsureClnup_mswin.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

enum TZI_ret // Timezone Category
{
	TzrInvalid = TIME_ZONE_ID_INVALID,   // -1
	TzrUnknown = TIME_ZONE_ID_UNKNOWN,   // 0
	TzrStandard = TIME_ZONE_ID_STANDARD, // 1
	TzrDaylight = TIME_ZONE_ID_DAYLIGHT, // 2
};

static void showmsg(HWND hdlg, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);
	vlSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, szfmt, args);
	va_end(args);
}

bool guiRefresh(HWND hdlg)
{
	showmsg(hdlg, _T(""));

	TIME_ZONE_INFORMATION tzi = {};
	TZI_ret tzr = (TZI_ret)GetTimeZoneInformation(&tzi);
	if(tzr==TzrInvalid)
	{
		showmsg(hdlg, _T("Unexpect! GetTimeZoneInformation() fail. WinErr=%s."), ITCS_WinError);
		return false;
	}

	vaSetDlgItemText(hdlg, IDS_TimezoneName, _T("Timezone name: %s"), tzi.StandardName);

	TCHAR szNewTzname[200] = {};
	GetDlgItemText(hdlg, IDE_NewTZName, szNewTzname, ARRAYSIZE(szNewTzname));
	if(!szNewTzname[0])
		vaSetDlgItemText(hdlg, IDE_NewTZName, _T("%s"), tzi.StandardName);

	vaSetDlgItemText(hdlg, IDE_NewTZBiasMinutes, _T("%d"), tzi.Bias);

	// Check token privileges 
	HANDLE hToken = NULL;
	BOOL succ = OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken);
	if(!succ) {
		showmsg(hdlg, _T("Unexpect! OpenProcessToken() fail. WinErr=%s."), ITCS_WinError);
		return false;
	}
	CEC_PTRHANDLE cec_hToken = hToken;

	PRIVILEGE_SET privset = {1, PRIVILEGE_SET_ALL_NECESSARY};
	
	BOOL enPrivSystime = 0;
	LookupPrivilegeValue(NULL, SE_SYSTEMTIME_NAME, &privset.Privilege[0].Luid);
	succ = PrivilegeCheck(hToken, &privset, &enPrivSystime);

	BOOL enPrivTimezone = 0;
	LookupPrivilegeValue(NULL, SE_TIME_ZONE_NAME, &privset.Privilege[0].Luid);
	succ = PrivilegeCheck(hToken, &privset, &enPrivTimezone);

	CheckDlgButton(hdlg, IDCHK_SeSystemtime, enPrivSystime);
	CheckDlgButton(hdlg, IDCHK_SeTimeZone, enPrivTimezone);

	return true;
}

bool adjustMyTokenPrivilege(HWND hdlg, const TCHAR *privname, BOOL isEnable)
{
	HANDLE hToken = NULL;
	BOOL succ = OpenProcessToken(GetCurrentProcess(), 
		TOKEN_QUERY|TOKEN_ADJUST_PRIVILEGES, &hToken);
	if(!succ) {
		showmsg(hdlg, _T("Unexpect! OpenProcessToken() fail. WinErr=%s."), ITCS_WinError);
		return false;
	}
	CEC_PTRHANDLE cec_hToken = hToken;

	TOKEN_PRIVILEGES tp = {1}; // count=1
	succ = LookupPrivilegeValue(NULL, privname, &tp.Privileges[0].Luid);
	if(!succ) {
		showmsg(hdlg, _T("%s is not supported on this system."), privname);
		return false;
	}

	if(isEnable)
		tp.Privileges[0].Attributes |= SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes &= ~SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	DWORD winerr = GetLastError(); // AdjustTokenPrivileges() returning TRUE means nothing.
	if(winerr) {
		// If cannot enable the privilege, we get WinErr=1300(ERROR_NOT_ALL_ASSIGNED).
		showmsg(hdlg, _T("AdjustTokenPrivileges() fail. WinErr=%s."), ITCS_WinError);
		return false;
	}

	return true;
}

void executeSetTimezone(HWND hdlg)
{
	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, _T(""));

	TIME_ZONE_INFORMATION tzi = {};
	TZI_ret tzr = (TZI_ret)GetTimeZoneInformation(&tzi);
	if(tzr==TzrInvalid)
	{
		showmsg(hdlg, _T("Unexpect! GetTimeZoneInformation() fail. WinErr=%d."), GetLastError());
		return;
	}

	BOOL isok = 0;
	LONG biasMinutes = GetDlgItemInt(hdlg, IDE_NewTZBiasMinutes, &isok, TRUE);
	if(!isok)
	{
		showmsg(hdlg, _T("Invalid input of Bias minutes. Must be a positive or negative integer."));
		return;
	}

	tzi.Bias = biasMinutes;
	GetDlgItemText(hdlg, IDE_NewTZName, tzi.StandardName, ARRAYSIZE(tzi.StandardName));

	BOOL succ = SetTimeZoneInformation(&tzi);
	if(succ)
	{
		vaMsgBox(hdlg, MB_OK, NULL, _T("SetTimeZoneInformation success."));
		guiRefresh(hdlg);
	}
	else
	{
		showmsg(hdlg, _T("SetTimeZoneInformation fail. WinErr=%s"), ITCS_WinError);
	}
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	switch (id) 
	{{
	case IDB_Refresh:
	{
		guiRefresh(hdlg);
		break;
	}
	case IDB_EXECUTE:
	{
		executeSetTimezone(hdlg);
		break;
	}
	case IDCHK_SeSystemtime:
	{
		BOOL isChecked = Button_GetCheck(GetDlgItem(hdlg, IDCHK_SeSystemtime));
		bool succ = adjustMyTokenPrivilege(hdlg, SE_SYSTEMTIME_NAME, !isChecked);
		if(succ)
			guiRefresh(hdlg);
		break;
	}
	case IDCHK_SeTimeZone:
	{
		BOOL isChecked = Button_GetCheck(GetDlgItem(hdlg, IDCHK_SeTimeZone));
		bool succ = adjustMyTokenPrivilege(hdlg, SE_TIME_ZONE_NAME, !isChecked);
		if(succ)
			guiRefresh(hdlg);
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
	JULayout *jul = JULayout::EnableJULayout(hdlg, 0,0, 0,1000);

 	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
 	jul->AnchorControl(0,100, 0,100, IDB_Refresh);
 	jul->AnchorControl(100,100, 100,100, IDB_EXECUTE);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	vaSetWindowText(hdlg, _T("guiSetTimeZone v%d.%d.%d"), 
		guiSetTimeZone_VMAJOR, guiSetTimeZone_VMINOR, guiSetTimeZone_VPATCH);
	
	Dlg_EnableJULayout(hdlg);

	if( guiRefresh(hdlg) )
	{
		showmsg(hdlg, 
			_T("Lab note: In order to make SetTimeZoneInformation success,\r\n")
			_T("* On Win8/7/Vista, enable SeTimeZonePrivilege above.\r\n")
			_T("* On WinXP, manually enable write access to regkey [HKLM\\SYSTEM\\CurrentControlSet\\Control\\TimeZoneInformation].")
			);
	}

	SetFocus(GetDlgItem(hdlg, IDB_EXECUTE));
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

	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, NULL);

	return 0;
}
