#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"

#include "iversion.h"

#include "utils.h"

#define JULAYOUT_IMPL
#include "JULayout2.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	bool isStarted;

	DWORD gtcStart;   // GetTickCount() start value, as milliseconds
	__int64 qpcStart; // QueryPerformanceCounter() start value, as CPU RDTSC
};

//#define TIMER_ID 100

void RefreshInfo(HWND hdlg)
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	DlgPrivate_st &ud = *prdata;
	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	DWORD gtcNow = GetTickCount();
	__int64 qpfNow = get_qpf();
	__int64 qpcNow = get_qpc();

	DWORD gtcDiff = gtcNow - ud.gtcStart;
	__int64 qpcDiff = qpcNow - ud.qpcStart;

	TCHAR tbuf[40] = {};
	vaSetDlgItemText(hdlg, IDC_EDIT_TimeElapsedByGetTickCount, 
		_T("%u.%03u seconds"), gtcDiff/1000, gtcDiff%1000 );
	SetDlgItemText(hdlg, IDC_EDIT_QPF, BigNum64ToString(get_qpf(), tbuf, _countof(tbuf)));
	SetDlgItemText(hdlg, IDC_EDIT_QPC, BigNum64ToString(get_qpc(), tbuf, _countof(tbuf)));

	DWORD qpcDiff_as_msec = DWORD( (qpcDiff*1000)/qpfNow );

	vaSetDlgItemText(hdlg, IDC_EDIT_TimeElapsedByQPC, 
		_T("%I64d / %I64d = %u.%03u (seconds)"),
		qpcDiff, qpfNow,    qpcDiff_as_msec/1000, qpcDiff_as_msec%1000);

	int bias_msec = qpcDiff_as_msec - gtcDiff;
	if(bias_msec >= 0)
	{
		vaSetDlgItemText(hdlg, IDC_EDIT_BiasSeconds, _T("%d.%03d"),
			bias_msec/1000, bias_msec%1000);
	}
	else
	{
		vaSetDlgItemText(hdlg, IDC_EDIT_BiasSeconds, _T("-%d.%03d"),
			-bias_msec/1000, -bias_msec%1000);
	}
}

VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent,	DWORD dwTime)
{
	HWND hdlg = (HWND)idEvent;
	assert(hdlg==hwnd);
	RefreshInfo(hdlg);
}

bool StartWork(HWND hdlg)
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	DlgPrivate_st &ud = *prdata;
	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	ud.gtcStart = GetTickCount();
	
	ud.qpcStart = get_qpc();
	if(ud.qpcStart == -1)
	{
		vaSetWindowText(hedit, 
			_T("QueryPerformanceCounter() fail. WinErr=%d"), GetLastError());
		return false;
	}

	SetTimer(hdlg, (UINT_PTR)hdlg, 100, TimerProc);
	return true;
}

void StopWork(HWND hdlg)
{
	KillTimer(hdlg, (UINT_PTR)hdlg);
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	DlgPrivate_st &ud = *prdata;

	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		if(! ud.isStarted)
		{
			if(StartWork(hdlg))
			{
				ud.isStarted = true;
				SetDlgItemText(hdlg, IDC_BUTTON1, _T("&Stop"));
			}
		}
		else
		{
			StopWork(hdlg);
			ud.isStarted = false;
			SetDlgItemText(hdlg, IDC_BUTTON1, _T("&Start"));
		}

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

	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
	jul->AnchorControl(50,100, 50,100, IDC_BUTTON1);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	vaSetWindowText(hdlg, _T("testQPC v%d.%d.%d"), 
		testQPC_VMAJOR, testQPC_VMINOR, testQPC_VPATCH);

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
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


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	DlgPrivate_st dlgdata = { };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)&dlgdata);

	return 0;
}
