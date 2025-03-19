#define OEMRESOURCE

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"
#include "iversion.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

void Using_LoadIcon(HWND hdlg)
{
	HMODULE hDll = LoadLibrary(_T("shell32.dll"));
	HICON hIco = LoadIcon(hDll, MAKEINTRESOURCE(243));

	HWND hBtn = GetDlgItem(hdlg, IDC_BUTTON1);
	HANDLE hPrevImg = (HANDLE)SendMessage(hBtn, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIco);
}

void Using_LoadImage_from_dll(HWND hdlg)
{
	HMODULE hDll = LoadLibrary(_T("shell32.dll"));
	
	HICON hIco = (HICON) LoadImage(hDll, MAKEINTRESOURCE(243), IMAGE_ICON,
		0, 0, // cxDesired, cyDesired
		LR_DEFAULTCOLOR |LR_SHARED
		);

	HWND hBtn = GetDlgItem(hdlg, IDC_BUTTON1);
	HANDLE hPrevImg = (HANDLE)SendMessage(hBtn, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIco);
}

void Using_LoadImage_from_icofile(HWND hdlg)
{
	HICON hIco = (HICON) LoadImage(NULL, _T("c64-128-32p.ico"), IMAGE_ICON,
		0, 0, // cxDesired, cyDesired
		LR_SHARED |LR_LOADFROMFILE
		);

	HWND hBtn = GetDlgItem(hdlg, IDC_BUTTON1);
	HANDLE hPrevImg = (HANDLE)SendMessage(hBtn, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIco);
}


inline bool IsCtrlKeyDown()
{
	SHORT keyCtrl = GetKeyState(VK_CONTROL); // "<0" means key pressed
	return keyCtrl<0 ? true : false;
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	TCHAR textbuf[200];

	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		++(prdata->clicks);
		_sntprintf_s(textbuf, _TRUNCATE, _T("Clicks: %d"), prdata->clicks);
		SetDlgItemText(hdlg, IDC_EDIT1, textbuf);

		if(IsCtrlKeyDown())
		{
			Using_LoadIcon(hdlg);
		}
		else
		{
			Using_LoadImage_from_icofile(hdlg);
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

// Sets the dialog box icons
inline void chSETDLGICONS(HWND hwnd, int idi) {
	SendMessage(hwnd, WM_SETICON, TRUE,  (LPARAM)
		LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		MAKEINTRESOURCE(idi)));
	SendMessage(hwnd, WM_SETICON, FALSE, (LPARAM)
		LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		MAKEINTRESOURCE(idi)));
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	chSETDLGICONS(hdlg, IDI_WINMAIN);

	SetDlgItemText(hdlg, IDC_EDIT1, 
		_T("Click the button to set button image.\r\n")
		_T("Simple click to use LoadImage().\r\n")
		_T("Ctrl+click to use LoadIcon().\r\n")
		);

	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 80,100, IDC_EDIT1);
	jul->AnchorControl(80,0, 100,50, IDC_BUTTON1);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));

	return 0; // Let Dlg-manager respect SetFocus().
}

INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_MSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_MSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) 
{
	g_hinstExe = hinstExe;

	DlgPrivate_st dlgdata = { _T("Hello.\r\nPrivate string here.") };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)&dlgdata);

	return 0;
}
