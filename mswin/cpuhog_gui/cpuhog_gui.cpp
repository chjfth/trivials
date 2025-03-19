#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#include "cpuhog.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#define VERSION "1.1"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

#define MY_TIMER_ID 1

static bool g_is_hog_started = false;
static DWORD g_start_msec = 0; // GetTickCount()
static HCURSOR g_cursorBusy, g_cursorArrow;

void Gui_StartCpuHog(HWND hdlg)
{
	int threads = GetDlgItemInt(hdlg, IDC_EDIT_THREADS, NULL, TRUE);
	int seconds = GetDlgItemInt(hdlg, IDC_EDIT_SECONDS, NULL, TRUE);
	HWND hbtn = GetDlgItem(hdlg, IDC_BTN_RUN);

	bool succ = start_cpu_hog(threads, seconds);
	assert(succ);

	g_start_msec = GetTickCount();

	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, _T("Start hogging..."));

//	SetCursor(g_cursorBusy); // No use calling it here.

	SetTimer(hdlg, MY_TIMER_ID, 500, NULL);

	Button_Enable(hbtn, FALSE);
	g_is_hog_started = true;
}

void Gui_StopCpuHog(HWND hdlg)
{
	HWND hbtn = GetDlgItem(hdlg, IDC_BTN_RUN);

	bool succ = done_cpu_hog();
	assert(succ);

//	SetCursor(g_cursorArrow); // No use calling it here.

	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, _T("Done hogging."));
	Button_Enable(hbtn, TRUE);
	g_is_hog_started = false;
}

void Dlg_OnTimer(HWND hdlg, UINT id)
{
	assert(id==MY_TIMER_ID);

	bool is_end = cpu_hog_is_end();
	if(is_end)
	{
		Gui_StopCpuHog(hdlg);
		KillTimer(hdlg, MY_TIMER_ID);
	}
	else
	{
		int elapsed_msec = GetTickCount() - g_start_msec;

		vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
			_T("Hogging for %d milliseconds..."), elapsed_msec);		
	}
}


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
//	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDC_BTN_RUN:
	{
		if(!g_is_hog_started)
			Gui_StartCpuHog(hdlg);
		else
			Gui_StopCpuHog(hdlg);

		break;
	}
	//case IDOK:
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

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	chSETDLGICONS(hdlg, IDI_WINMAIN);

	g_cursorArrow = LoadCursor(NULL, IDC_ARROW);
	g_cursorBusy = LoadCursor(NULL, IDC_WAIT);

	vaSetWindowText(hdlg, _T("cpuhog GUI v%s"), _T(VERSION));

	SetDlgItemInt(hdlg, IDC_EDIT_THREADS, 1, TRUE);
	SetDlgItemInt(hdlg, IDC_EDIT_SECONDS, 2, TRUE);

	vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG,
		_T("This program hogs CPU times. \r\n")
		_T("You can assign threads(max %d) and seconds to hog.")
		,
		MAX_THREADS
		);

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_EDIT_THREADS));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

BOOL Dlg_OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT mousemsg)
{
	if(codeHitTest==HTCLIENT && g_is_hog_started)
	{
		SetCursor(g_cursorBusy);
		return TRUE; 
		// -- TRUE: so that the Busy-cursor *persists*, whether on dlgbox's blank area,
		//          or on editbox 's area.
	}
	else
	{
		// We obey system's default mouse-cursor strategy,
		// by calling DefWindowProc() for this WM_CURSOR.
		return FORWARD_WM_SETCURSOR(hwnd, hwndCursor, codeHitTest, mousemsg, DefWindowProc);
	}
}


INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
		HANDLE_dlgMSG(hdlg, WM_TIMER, Dlg_OnTimer);
		HANDLE_dlgMSG(hdlg, WM_SETCURSOR, Dlg_OnSetCursor);
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
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)&dlgdata);

	return 0;
}
