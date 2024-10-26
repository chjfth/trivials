#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "utils.h"
#include "dlptr_winapi.h"

#define JULAYOUT_IMPL
#include "JULayout2.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

#define APPNAME "WM_TIMER_accuracy"

#define TIMER_ID 1

struct DlgPrivate_st
{
	bool isProbeStarted;

	// user UI input
	int count_max; 
	int wm_timer_ms;
	int is_stophustle;
	
	DWORD startTickMillisec;
	DWORD prevTickMillisec;

	int count;
	int min_step_ms;
	int max_step_ms;
};

void DoTimerStop(HWND hdlg, DlgPrivate_st *prdata)
{
	if(prdata->isProbeStarted==false)
	{
		// We need this, bcz vaMsgBox() may call OnTimer() -> DoTimerStop() nested.
		return;
	}

	KillTimer(hdlg, TIMER_ID);
	prdata->isProbeStarted = false;

	DWORD endTickMillisec = GetTickCount();
	int elapsed_mils = endTickMillisec - prdata->startTickMillisec;

	if(elapsed_mils>=100)
	{
		HWND hedit = GetDlgItem(hdlg, IDC_EDIT_RUNINFO);
		vaAppendText_mled(hedit, 
			_T("Got %d timer messages in total %g seconds, average %g ms per message."), 
			prdata->count, elapsed_mils/1000.0, (double)elapsed_mils/prdata->count);
	}

	SetDlgItemText(hdlg, IDC_BUTTON1, _T("&Start Probe"));
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	TCHAR tbuf[200];

	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		if(!prdata->isProbeStarted)
		{
			BOOL isTrans = FALSE;
			UINT millisec = GetDlgItemInt(hdlg, IDC_EDIT_TimerMillisec, &isTrans, FALSE);
			if(!isTrans)
			{
				GetDlgItemText(hdlg, IDC_EDIT_TimerMillisec, tbuf, ARRAYSIZE(tbuf)-1);
				vaMsgBox(hdlg, MB_OK|MB_ICONASTERISK, _T(APPNAME),
					_T("Wrong millisec input: %s"), tbuf);
				return;
			}

			SetTimer(hdlg, TIMER_ID, millisec, NULL);

			SetDlgItemText(hdlg, IDC_EDIT_RUNINFO, _T(""));

			prdata->isProbeStarted = true;
			prdata->count = 0;
			prdata->count_max = GetDlgItemInt(hdlg, IDC_EDIT_RunCount, NULL, FALSE);
			prdata->is_stophustle = Button_GetCheck(GetDlgItem(hdlg, IDC_CHECK_ReportHustleTick));
			prdata->wm_timer_ms = millisec;
			prdata->min_step_ms = millisec + 1000;
			prdata->max_step_ms = 0;
			prdata->prevTickMillisec = prdata->startTickMillisec = GetTickCount();
			
			SetDlgItemText(hdlg, IDC_BUTTON1, _T("&Stop Probe"));
		}
		else
		{
			DoTimerStop(hdlg, prdata);
		}
		
		break;
	}
	case IDCANCEL:
	{
		EndDialog(hdlg, id);
		break;
	}
	}}
}

// Sets the dialog box icons
inline void chSETDLGICONS(HWND hwnd, int idi) {
	SendMessage(hwnd, WM_SETICON, TRUE,  (LPARAM)
		LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		MAKEINTRESOURCE(idi)));
	SendMessage(hwnd, WM_SETICON, FALSE, (LPARAM)
		LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		MAKEINTRESOURCE(idi)));
}

typedef LONG NTSTATUS;
NTSTATUS WINAPI NtQueryTimerResolution(
	ULONG* MinimumResolution,
	ULONG* MaximumResolution,
	ULONG* CurrentResolution
);
DEFINE_DLPTR_WINAPI("ntdll.dll", NtQueryTimerResolution)

void TellTimerResolution(HWND hdlg)
{
	TCHAR textbuf[100] = _T("ntdll!NtQueryTimerResolution not available.");
	if(dlptr_NtQueryTimerResolution)
	{
		ULONG minRes=0, maxRes=0, curRes=0; // unit 0.1 usec
		NTSTATUS ntserr = dlptr_NtQueryTimerResolution(&minRes, &maxRes, &curRes);
		if(!ntserr)
		{
			_sntprintf_s(textbuf, _TRUNCATE, 
				_T("NtQueryTimerResolution(ms): current: %g  (min: %g , max: %g)"),
				curRes/10000.0, minRes/10000.0, maxRes/10000.0);
		}
		else
		{
			_sntprintf_s(textbuf, _TRUNCATE, 
				_T("Fail calling ntdll!NtQueryTimerResolution(), ntstatus=%d"), 
				ntserr);
		}
	}
	
	SetDlgItemText(hdlg, IDC_LBL_TimerResolution, textbuf);
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	chSETDLGICONS(hdlg, IDI_WINMAIN);

	vaSetWindowText(hdlg, _T("WM_TIMER_accuracy v%d.%d"), 
		WM_TIMER_accuracy_VMAJOR, WM_TIMER_accuracy_VMINOR);

	SetDlgItemText(hdlg, IDC_EDIT_RUNINFO, _T("Probing progress appears here."));
	SetDlgItemText(hdlg, IDC_LBL_Result, _T(""));

	TellTimerResolution(hdlg);

	SetDlgItemInt(hdlg, IDC_EDIT_TimerMillisec, 50, FALSE);
	SetDlgItemInt(hdlg, IDC_EDIT_RunCount, 1000, FALSE);

	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_LBL_TimerResolution);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_RUNINFO);
	jul->AnchorControl(0,100, 0,100, IDC_LBL_Result);
	jul->AnchorControl(50,100, 50,100, IDC_BUTTON1);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

void Dlg_OnTimer(HWND hdlg, UINT timerid)
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	assert(prdata->isProbeStarted);

	prdata->count++;
	DWORD nowtick = GetTickCount();
	
	int step_ms = nowtick-prdata->prevTickMillisec;

	{
		// Append text with length limit:

		HWND hedit = GetDlgItem(hdlg, IDC_EDIT_RUNINFO);

		int textlen = Edit_GetTextLength(hedit);
		if(textlen>20000)
			SetWindowText(hedit, _T("")); // Clear old text

		vaAppendText_mled(hedit, _T("[#%d] +%u ms\r\n"), prdata->count, step_ms);
	}

	prdata->prevTickMillisec = nowtick;

	if(step_ms < prdata->min_step_ms)
		prdata->min_step_ms = step_ms;

	if(step_ms > prdata->max_step_ms)
		prdata->max_step_ms = step_ms;

	vaSetDlgItemText(hdlg, IDC_LBL_Result, _T("Timer interval(ms): min: %d , max: %d"),
		prdata->min_step_ms, prdata->max_step_ms);

	if(prdata->is_stophustle && (step_ms < prdata->wm_timer_ms))
	{
		// Note: When the message box pops out, the WM_TIMER is still generated.
		// So to preserve the spot, we tweak prdata->count_max to make it stop prematurely.
		//
		prdata->count_max = prdata->count + 10;

		vaMsgBox(hdlg, MB_OK|MB_ICONEXCLAMATION, _T(APPNAME), 
			_T("Unexpected! [#%d]Got a timer interval(%d ms) LESS THAN user requested(%d ms).")
			_T("\r\n\r\n")
			_T("Probing will stop prematurely.")
			,
			prdata->count, step_ms, prdata->wm_timer_ms);
	}

	if(prdata->count == prdata->count_max)
	{
		DoTimerStop(hdlg, prdata);
	}
}


INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
		HANDLE_dlgMSG(hdlg, WM_TIMER,         Dlg_OnTimer);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	InitCommonControls();

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);


	DlgPrivate_st dlgdata = { };
	dlgdata.isProbeStarted = false;
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)&dlgdata);

	return 0;
}
