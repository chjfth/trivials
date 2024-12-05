#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include "resource.h"
#include "iversion.h"
#include "../utils.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HWND g_hdlgVarFont;
HWND g_hdlgSysFont;

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PTSTR szCmdLine, int nCmdShow)
{
	InitCommonControls(); // WinXP needs this

	(void)hPrevInstance; (void)szCmdLine; 
	static TCHAR szAppName[] = TEXT ("DlgBaseUnits") ;
	HWND         hwnd = NULL;
	MSG          msg = {};
	WNDCLASS     wndclass = {};

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	RegisterClass (&wndclass);
	 
	hwnd = CreateWindow (szAppName,    // window class name
		TEXT ("The DlgBaseUnits Program"), // window caption
		WS_OVERLAPPEDWINDOW,           // window style
		20,              // initial x position
		20,              // initial y position
		400,             // initial x size
		200,             // initial y size
		NULL,            // parent window handle
		NULL,            // window menu handle
		hInstance,       // program instance handle
		NULL) ;          // creation parameters
	 
	SendMessage(hwnd, WM_SETICON, TRUE, (LPARAM)LoadIcon(hInstance,	MAKEINTRESOURCE(IDI_WINMAIN)));

	g_hdlgVarFont = CreateDialog(hInstance, 
		MAKEINTRESOURCE(IDD_VARFONT), hwnd, Dlg_Proc);

	ShowWindow (hwnd, nCmdShow) ;
	UpdateWindow (hwnd) ;
	
	while (GetMessage (&msg, NULL, 0, 0))
	{
		if(IsDialogMessage(g_hdlgVarFont, &msg))
			continue;

		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	return (int)msg.wParam; // the value N told by PostQuitMessage(N);
}

BOOL Cls_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	return TRUE; // success, go on creation
}

void Cls_OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps = {};
	RECT        rect ;
	HDC hdc = BeginPaint (hwnd, &ps) ;

	GetClientRect (hwnd, &rect) ;          
	Ellipse(hdc, 0,0, rect.right, rect.bottom);
	DrawText (hdc, TEXT ("Hello, WindowsX !"), -1, &rect,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;

	EndPaint (hwnd, &ps) ;
}

void Cls_OnDestroy(HWND hwnd)
{
	PostQuitMessage(44);
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{{
		HANDLE_MSG(hwnd, WM_CREATE, Cls_OnCreate);
		HANDLE_MSG(hwnd, WM_PAINT, Cls_OnPaint);
		HANDLE_MSG(hwnd, WM_DESTROY, Cls_OnDestroy);
	}}
	
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

//////////////////////////////////////////////////////////////////////////

void ShowDlgBaseUnits(HWND hdlg)
{
	ULONG bus = GetDialogBaseUnits();

	int dlgx = 4, dlgy = 8;
	RECT rc = {0,0, dlgx,dlgy};
	BOOL succ = MapDialogRect(hdlg, &rc);
	assert(succ);

	vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
		_T("GetDialogBaseUnits() = [%d,%d]\r\n")
		_T("MapDialogRect(x=%d, y=%d) = [%d,%d]\r\n")
		,
		LOWORD(bus), HIWORD(bus),
		dlgx, dlgy, rc.right, rc.bottom
		);
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	switch (id) 
	{
	case IDC_BTN_REFRESH:
		ShowDlgBaseUnits(hdlg);
		break;
	
	case IDOK:
	case IDCANCEL:
		EndDialog(hdlg, id);
		break;
	}
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		DlgBaseUnits_VMAJOR, DlgBaseUnits_VMINOR, DlgBaseUnits_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);

	ShowDlgBaseUnits(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_BTN_REFRESH));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	vaDbgTs(_T("Dlg_Proc(0x%08X): msg=%d"), hdlg, uMsg);
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
	}
	return FALSE;
}
