#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

bool g_iscapture = false;
HWND g_hedit;


struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDOK:
	case IDCANCEL:
		EndDialog(hdlg, id);
		break;
	}}
}

static void appendmsg(TCHAR *fmt, ...)
{
	TCHAR timestr[40] = {};
	vaAppendText_mled(g_hedit, _T("%s "), now_timestr(timestr, ARRAYSIZE(timestr)));

	va_list args;
	va_start(args, fmt);
	vlAppendText_mled(g_hedit, fmt, args);
	va_end(args);

	vaAppendText_mled(g_hedit, _T("\r\n"));
}

void Dlg_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	appendmsg(L"mouse down");

	g_iscapture = true;
	SetCapture(hwnd);
	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_CROSS)));
}

void Dlg_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	appendmsg(L"mouse up");

	g_iscapture = false;
	ReleaseCapture();
	SetCursor(LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
}

void Dlg_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	static HWND s_hwnd_lasttime = NULL;

	if(!g_iscapture)
		return;

	POINT pt = {x, y};
	ClientToScreen(hwnd, &pt);

	HWND hwnd_target = WindowFromPoint(pt);
	if(hwnd_target!=s_hwnd_lasttime)
	{
		s_hwnd_lasttime = hwnd_target;

		if(hwnd_target==NULL)
		{
			appendmsg(_T("(%d, %d) -> NULL hwnd!"), pt.x, pt.y);
			return;
		}

		TCHAR info_root[40] = {};
		HWND hwndRoot = GetAncestor(hwnd_target, GA_ROOT);
		if(hwndRoot && hwndRoot!=hwnd_target)
		{
			_sntprintf_s(info_root, _TRUNCATE, _T(", GA_ROOT=0x%08X"), (UINT)hwndRoot);
		}

		appendmsg(_T("(%d, %d) -> hwnd: 0x%08X %s"),
			pt.x, pt.y, (UINT)hwnd_target, info_root);
	}

}


static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_TEXT1);
	jul->AnchorControl(100,0, 100,0, IDOK);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT1);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	vaSetWindowText(hdlg, _T("testWindowFromPoint v%d.%d.%d"), 
		testWindowFromPoint_VMAJOR, testWindowFromPoint_VMINOR, testWindowFromPoint_VPATCH);
	
	Dlg_EnableJULayout(hdlg);

	SetDlgItemText(hdlg, IDC_TEXT1, _T("Click on me and drag over other windows."));

	g_hedit = GetDlgItem(hdlg, IDC_EDIT1);

//	SetFocus(GetDlgItem(hdlg, IDOK));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR WINAPI UserDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);

		HANDLE_dlgMSG(hdlg, WM_LBUTTONDOWN, Dlg_OnLButtonDown);
		HANDLE_dlgMSG(hdlg, WM_LBUTTONUP, Dlg_OnLButtonUp);
		HANDLE_dlgMSG(hdlg, WM_MOUSEMOVE, Dlg_OnMouseMove);
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
