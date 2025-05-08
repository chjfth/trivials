/* This program demonstrate the window-class superclassing idea from :

https://www.drdobbs.com/windows/windows-95-subclassing-and-superclassing/184410048

In SuperclassEditbox.rc, we define two custom controls of wndclass-name "NodigitEditbox".
We build "NodigitEditbox" on the basis of system-provided "edit" wndclass.

Key functions:
* make_NodigitEditbox_wndclass()
* wndproc_NodigitEditbox()

*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <mswin/WindowsX2.h> // chj: for SUBCLASS_FILTER_MSG0()
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"

#include "iversion.h"

#include "..\utils.h"

#include <mswin/WinError.itc.h>
using namespace itc;

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

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

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);

	vaSetDlgItemText(hdlg, IDC_LABEL1, _T("%s"),
		_T("The two editbox below accept only non-digit text.\r\n")
		_T(".rc DIALOGEX statement refers to classname \"NodigitEditbox\".")
		);
	
	SetFocus(GetDlgItem(hdlg, IDC_EDIT1));
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

//////////////////////////////////////////////////////////////////////////

WNDCLASSEX g_wndclass_edit;

MsgRelay_et NodigitEditbox_OnChar(HWND hwnd, TCHAR ch, int cRepeat)
{
	// Note: This does not prevent user Ctrl+V to paste in digit text.

	USHORT chu = 0;
	if(sizeof(ch)==1)
		chu = (UCHAR)ch;
	else
		chu = (USHORT)ch;

	if(ch>='0' && ch<='9')
	{
		vaDbgTs(_T("[NodigitEditbox@%08X] sees digit char %c (0x%X), ignore it."), 
			hwnd, ch, chu);
		MessageBeep(0);
		return MsgRelay_no;
	}
	else
	{
		vaDbgTs(_T("[NodigitEditbox@%08X] sees normal char %c (0x%X)."), 
			hwnd, ch, chu);
		return MsgRelay_yes;
	}
}

LRESULT WINAPI wndproc_NodigitEditbox(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg)
	{
		SUBCLASS_FILTER_MSG0(hwnd, WM_CHAR, NodigitEditbox_OnChar);
	}

	return CallWindowProc(g_wndclass_edit.lpfnWndProc, hwnd, uMsg, wParam, lParam);
}

void make_NodigitEditbox_wndclass(HINSTANCE hinstExe)
{
	WNDCLASSEX wndclass = {sizeof(wndclass)};
	ATOM atom_old = GetClassInfoEx(NULL, _T("edit"), &wndclass);

	g_wndclass_edit = wndclass; // save old editbox-class info

	wndclass.lpfnWndProc = wndproc_NodigitEditbox;
	wndclass.cbClsExtra += 4 * sizeof(void*);
	wndclass.cbWndExtra += 4 * sizeof(void*); // arbitrary test
	wndclass.lpszClassName = _T("NodigitEditbox");

	ATOM atom_new = RegisterClassEx(&wndclass);
	if(atom_new==0)
	{
		vaMsgBox(NULL, MB_OK|MB_ICONERROR, NULL, 
			_T("RegisterClassEx() fail, winerr=%s"), ITCS_WinError);
		exit(44);
	}

}

int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	make_NodigitEditbox_wndclass(hinstExe);

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	DlgPrivate_st dlgdata = { _T("Hello.\r\nPrivate string here.") };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, (LPARAM)&dlgdata);

	return 0;
}
