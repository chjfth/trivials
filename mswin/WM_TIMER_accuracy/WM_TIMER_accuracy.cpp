#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "..\utils.h"
#include <mswin/dlptr_winapi.h>

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#pragma warning(disable:4800)  // warning C4800: 'int' : forcing value to bool 'true' or 'false' (performance warning)

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

#define APPNAME "WM_TIMER_accuracy"

#define TIMER_ID 1

struct DlgPrivate_st
{
	bool isProbeStarted;

	// user UI input
	int timer_count_max; 
	int wm_timer_ms;
	bool is_stophustle;
	int sleepms; // Sleep millisec in WM_TIMER callback
	bool is_sleep_onlyonce;
	
	DWORD startTickMillisec;
	DWORD prevTickMillisec;

	int timer_count;
	int sleep_count;
	int sleep_max;
	int min_step_ms;
	int max_step_ms;
};


void DlgItem_Enable(HWND hdlg, int nIDDlgItem, bool en)
{
	EnableWindow(GetDlgItem(hdlg, nIDDlgItem), en?TRUE:FALSE);
}

void EnableDisableInputUic(HWND hdlg, bool en)
{
	DlgItem_Enable(hdlg, IDC_EDIT_TimerMillisec, en);
	DlgItem_Enable(hdlg, IDC_EDIT_RunCount, en);
	DlgItem_Enable(hdlg, IDC_CHECK_ReportHustleTick, en);
	DlgItem_Enable(hdlg, IDC_EDIT_Sleepms, en);
}

void DoTimerStart(HWND hdlg, DlgPrivate_st *prdata)
{
	TCHAR tbuf[200];

	BOOL isTrans = FALSE;
	UINT millisec = GetDlgItemInt(hdlg, IDC_EDIT_TimerMillisec, &isTrans, FALSE);
	if(!isTrans)
	{
		GetDlgItemText(hdlg, IDC_EDIT_TimerMillisec, tbuf, ARRAYSIZE(tbuf)-1);
		vaMsgBox(hdlg, MB_OK|MB_ICONASTERISK, _T(APPNAME),
			_T("Wrong millisec input: %s"), tbuf);
		return;
	}

	prdata->isProbeStarted = true;
	prdata->timer_count = 0;
	prdata->timer_count_max = GetDlgItemInt(hdlg, IDC_EDIT_RunCount, NULL, FALSE);
	prdata->is_stophustle = (bool)Button_GetCheck(GetDlgItem(hdlg, IDC_CHECK_ReportHustleTick));
	prdata->sleepms = GetDlgItemInt(hdlg, IDC_EDIT_Sleepms, NULL, TRUE);
	prdata->is_sleep_onlyonce = (bool)Button_GetCheck(GetDlgItem(hdlg, IDC_CHECK_SleepOnlyOnce));
	prdata->sleep_count = 0;
	prdata->wm_timer_ms = millisec;
	prdata->min_step_ms = millisec + 1000;
	prdata->max_step_ms = 0;
	prdata->prevTickMillisec = prdata->startTickMillisec = TrueGetMillisec();

	SetDlgItemText(hdlg, IDC_EDIT_RUNINFO, _T(""));

	SetDlgItemText(hdlg, IDOK, _T("&Stop Probe"));

	EnableDisableInputUic(hdlg, false);

	SetTimer(hdlg, TIMER_ID, millisec, NULL);
}

void DoTimerStop(HWND hdlg, DlgPrivate_st *prdata)
{
	if(prdata->isProbeStarted==false)
	{
		// We need this, bcz vaMsgBox() may call OnTimer() -> DoTimerStop() nested.
		return;
	}

	KillTimer(hdlg, TIMER_ID);
	prdata->isProbeStarted = false;

	DWORD endTickMillisec = TrueGetMillisec();
	int elapsed_mils = endTickMillisec - prdata->startTickMillisec;

	if(elapsed_mils>=100)
	{
		HWND hedit = GetDlgItem(hdlg, IDC_EDIT_RUNINFO);
		vaAppendText_mled(hedit, 
			_T("Got %d timer messages in total %g seconds, average %g ms per message."), 
			prdata->timer_count, elapsed_mils/1000.0, (double)elapsed_mils/prdata->timer_count);
	}

	EnableDisableInputUic(hdlg, true);

	SetDlgItemText(hdlg, IDOK, _T("&Start Probe"));
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDOK:
	{
		if(!prdata->isProbeStarted)
		{
			DoTimerStart(hdlg, prdata);
		}
		else
		{
			DoTimerStop(hdlg, prdata);
		}
		
		break;
	}
	case IDCANCEL:
	{
		// [2024-10-27] I do NOT call EndDialog() here, just as a special code demo.
		// I do not want ESC key to close the dialog(=End whole exe).
		// Instead, I call EndDialog() in Dlg_FilterSysCommand(), which can be triggered
		// by click window-title Close[X] button.
		// EndDialog(hdlg, id);
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

void Tell_HardIntr_TimerResolution(HWND hdlg)
{
	TCHAR textbuf[100] = _T("ntdll!NtQueryTimerResolution not available.");
	if(dlptr_NtQueryTimerResolution)
	{
		ULONG minRes=0, maxRes=0, curRes=0; // unit 0.1 usec
		NTSTATUS ntserr = dlptr_NtQueryTimerResolution(&minRes, &maxRes, &curRes);
		if(!ntserr)
		{
			_sntprintf_s(textbuf, _TRUNCATE, 
				_T("NtQueryTimerResolution(millisec): current: %g  (min: %g , max: %g)"),
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

const TCHAR *gets_QueueStatus(DWORD qs, TCHAR *tbuf, int bufsize)
{
	_sntprintf_s(tbuf, bufsize, _TRUNCATE, _T("QS=0x%08X"), qs);
	
	if(HIWORD(qs) & QS_TIMER)
	{
		_sntprintf_s(tbuf, bufsize, _TRUNCATE, _T("%s QS_TIMER"), tbuf);
	}
	return tbuf;
}

void Dlg_OnTimer(HWND hdlg, UINT timerid)
{
	TCHAR tbuf[100] = {};
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	assert(prdata->isProbeStarted);

	prdata->timer_count++;
	DWORD nowtick = TrueGetMillisec();

	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_RUNINFO);
	int textlen = Edit_GetTextLength(hedit);
	if(textlen>20000)
		SetWindowText(hedit, _T("")); // Clear old overly long text

	int step_ms = nowtick-prdata->prevTickMillisec;

	{
		// Append text with length limit:

		DWORD qs = GetQueueStatus(QS_TIMER);
		if(qs)
		{
			// During normal run(not delayed by a debugger at Dlg_Timer() entrance),
			// we should not meet this.
			TCHAR tbuf2[40] = {};
			gets_QueueStatus(qs, tbuf2, ARRAYSIZE(tbuf2));
			_sntprintf_s(tbuf, _TRUNCATE, _T(" (%s)"), tbuf2);
		}

		vaAppendText_mled(hedit, _T("[#%d] +%u ms") _T("%s\r\n"), 
			prdata->timer_count, step_ms, tbuf);
	}

	prdata->prevTickMillisec = nowtick;

	if(step_ms < prdata->min_step_ms)
		prdata->min_step_ms = step_ms;

	if(step_ms > prdata->max_step_ms)
		prdata->max_step_ms = step_ms;

	vaSetDlgItemText(hdlg, IDC_LBL_MinMax, _T("Timer interval(ms): min: %d , max: %d"),
		prdata->min_step_ms, prdata->max_step_ms);

	if(prdata->is_stophustle && (step_ms < prdata->wm_timer_ms))
	{
		// Note: When the message box pops out, the WM_TIMER is still generated.
		// So to preserve the spot, we tweak prdata->count_max to make it stop prematurely.
		//
		prdata->timer_count_max = Min(prdata->timer_count + 10, prdata->timer_count_max);

		vaMsgBox(hdlg, MB_OK|MB_ICONEXCLAMATION, _T(APPNAME), 
			_T("Unusual! [#%d]Got a timer interval(%d ms) LESS THAN user requested(%d ms).")
			_T("\r\n\r\n")
			_T("Probing will stop prematurely soon.")
			,
			prdata->timer_count, step_ms, prdata->wm_timer_ms);
	}

	// Extra Sleep()

	if(prdata->sleepms >= 0)
	{
		if(prdata->is_sleep_onlyonce==false || prdata->sleep_count==0)
		{
			prdata->sleep_count++;

//		ShowWindow(GetDlgItem(hdlg, IDC_LBL_Sleeping), SW_SHOW); // seems cached by system
			SetDlgItemText(hdlg, IDC_LBL_Sleeping, _T("<sleeping>"));
			Sleep(prdata->sleepms);
			SetDlgItemText(hdlg, IDC_LBL_Sleeping, _T(""));
//		ShowWindow(GetDlgItem(hdlg, IDC_LBL_Sleeping), SW_HIDE);

			DWORD qs = GetQueueStatus(QS_TIMER);
			if(qs)
			{
				// We will see this if Sleep() longer than WM_TIMER interval.
				gets_QueueStatus(qs, tbuf, ARRAYSIZE(tbuf));
				vaAppendText_mled(hedit, _T("    After Sleep(), %s\r\n"),
					gets_QueueStatus(qs, tbuf, ARRAYSIZE(tbuf)));
			}

		}
	}

	if(prdata->timer_count == prdata->timer_count_max)
	{
		DoTimerStop(hdlg, prdata);
	}
}

static DWORD wait_tickcount_change()
{
	DWORD ms1 = GetTickCount();
	DWORD ms2 = 0;
	while( (ms2=GetTickCount()) == ms1 );
	return ms2;
}

static UINT64 wait_perft_change()
{
	LARGE_INTEGER li1 = {};
	QueryPerformanceCounter(&li1);
	UINT64 us1 = li1.QuadPart / 10;

	for(;;)
	{
		LARGE_INTEGER li2 = {};
		QueryPerformanceCounter(&li2);
		UINT64 us2 = li2.QuadPart / 10;

		if(us2 != us1)
			return us2; // return microseconds
	}
}

static void show_timer_resolution(HWND hdlg)
{
	DWORD ms1 = wait_tickcount_change();
	DWORD ms2 = wait_tickcount_change();
	
	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_RUNINFO);
	vaAppendText_mled(hedit, _T("\r\nGetTickCount() resolution is: %d millisec.\r\n"), ms2-ms1);

	UINT64 us1 = wait_perft_change();
	UINT64 us2 = wait_perft_change();
	vaAppendText_mled(hedit, _T("QueryPerformanceCounter() resolution is: %I64d microsec.\r\n"), us2-us1);

	vaAppendText_mled(hedit, _T("\r\nI will use QueryPerformanceCounter to measure WM_TIMER."));

	Tell_HardIntr_TimerResolution(hdlg);
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);

	chSETDLGICONS(hdlg, IDI_WINMAIN);

	vaSetWindowText(hdlg, _T("WM_TIMER_accuracy v%d.%d.%d"), 
		WM_TIMER_accuracy_VMAJOR, WM_TIMER_accuracy_VMINOR, WM_TIMER_accuracy_VPATCH);

	// Set Initial Uic content:

	SetDlgItemInt(hdlg, IDC_EDIT_TimerMillisec, 50, FALSE);
	SetDlgItemInt(hdlg, IDC_EDIT_RunCount, 1000, FALSE);
	SetDlgItemInt(hdlg, IDC_EDIT_Sleepms, -1, TRUE);
	SetDlgItemText(hdlg, IDC_LBL_Sleeping, _T(""));
	SetDlgItemText(hdlg, IDC_LBL_MinMax, _T(""));
	SetDlgItemText(hdlg, IDC_EDIT_RUNINFO, 
		_T("The program probes WM_TIMER's actual timing behavior by checking GetTickCount() timestamps.")
		_T("\r\n\r\n")
		_T("Parameter hint: \r\n")
		_T("- If Sleep millisec is -1, then I will NOT call Sleep() in WM_TIMER callback.\r\n")
		_T("- If Sleep millisec is 0, then I will call Sleep(0) in WM_TIMER callback.\r\n")
		_T("- If Sleep millisec is 500, then I will call Sleep(500) in WM_TIMER callback, just before our DlgProc returns.\r\n")
		);

	show_timer_resolution(hdlg);

	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_LBL_TimerResolution);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_RUNINFO);
	jul->AnchorControl(0,100, 0,100, IDC_LBL_MinMax);
	jul->AnchorControl(50,100, 50,100, IDOK);

	SetFocus(GetDlgItem(hdlg, IDC_EDIT_TimerMillisec));

	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

void Dlg_HookSysCommand(HWND hwnd, UINT cmd, int x, int y)
{
	if(cmd==SC_CLOSE)
	{
		EndDialog(hwnd, 0);
	}
}

INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
		HANDLE_dlgMSG(hdlg, WM_TIMER,         Dlg_OnTimer);

		// [2024-10-27] For WM_SYSCOMMAND, must use HANDLE_MSG() instead of HANDLE_dlgMSG().
		// HANDLE_dlgMSG() will return 1, which will bypass DefWindowProc() WM_SYSCOMMAND processing,
		// defeating all intrinsic window behaviors, like "window cannot move by mouse".
		HANDLE_MSG(hdlg, WM_SYSCOMMAND, Dlg_HookSysCommand); // return 0
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
