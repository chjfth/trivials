#define WIN32_LEAN_AND_MEAN
#include <WindowsX.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "CmnHdr-Jeffrey.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "utils.h"
#include "Checker2m.h"

///////////////////////////////////////////////////////////////////////////////

struct DlgPrivate_st
{
	const WCHAR *mystr;
};

BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	chSETDLGICONS(hwnd, IDI_WINMAIN);

	DlgPrivate_st *pr = new DlgPrivate_st;
	pr->mystr = (const WCHAR*)lParam;

	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)pr);

	SetDlgItemText(hwnd, IDC_LBL_HIDDEN, L"");
	const WCHAR *text_banner = 
L"Click a button to launch a thread.\r\n"
L"\r\n"
L" >> For first button, the thread will call UI function directly;\r\n"
L" >> For second button, marshaling call is carried out.\r\n"
L"\r\n"
L"(TODO) Run this program with VS debugger attached, the first button will cause "
L"'Cross-thread operation not valid' exception. and the second will be fine.\r\n"
	;
	SetDlgItemText(hwnd, IDC_BANNER_TEXT, text_banner);

//	HWND hwndRootKey = GetDlgItem(hwnd, IDC_ROOTKEY);

	Checker2_RegisterClass();

	return(TRUE);
}

void Dlg_OnDestroy(HWND hwnd)
{
	DlgPrivate_st *pr = (DlgPrivate_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	// Destroy all resources allocated back in Dlg_OnInitDialog().
	delete pr;
}

int thread_DirectUI(void *param)
{
	HWND hwnd = (HWND)param;
	SetDlgItemText(hwnd, IDC_LBL_MESSAGE, L"Direct text from worker thread.");
	return 0;
}

int thread_extraGUI(void *param)
{
	Checker2_RunGUIThread();
	return 0;
}

void CreateGUIThreads(HWND hParent)
{
	// Chj:
	// Here I create two worker threads who respectively creates a checker window.
	// We will see that: if the thread quits pre-maturely(break out of message loop code),
	// its associating window is destroyed by system automatically.

	HANDLE arth[2] = {};
	arth[0] = winCreateThread(thread_extraGUI, NULL);
	arth[1] = winCreateThread(thread_extraGUI, NULL);
	
	DWORD waitre = WaitForMultipleObjects(2, arth, TRUE, INFINITE);

	MessageBox(hParent, L"2 GUI threads done.", L"Info", MB_OK);
}

void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{
//	DlgPrivate_st *pr = (DlgPrivate_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	switch (id) 
	{{
	case IDOK:
	case IDCANCEL:
		EndDialog(hwnd, id);
		break;
	
	case ID_BTN_UIDIRECT:
	{
		HANDLE hThread = winCreateThread(thread_DirectUI, hwnd);

		SetDlgItemText(hwnd, IDC_LBL_MESSAGE, L"Created the DirectUI thread. Waiting 3 seconds for thread end...");
		DWORD thread_ret = winWaitThreadEnd(hThread, 3000); (void)thread_ret;

		if (thread_ret == 0) {
			SetDlgItemText(hwnd, IDC_LBL_MESSAGE, L"DirectUI thread done.");
		}
		else {
			SetDlgItemText(hwnd, IDC_LBL_MESSAGE, L"DirectUI thread not finished yet.");
			Sleep(1000);
		}

		(void)hThread; 
		break;
	}
	case ID_BTN_UIMARSHAL:
	{	CreateGUIThreads(hwnd);
		break;
	}
	}}
}



void Dlg_OnSize(HWND hwnd, UINT state, int cx, int cy) 
{
	// Reposition the child controls
//	g_UILayout.AdjustControls(cx, cy);    
}


void Dlg_OnGetMinMaxInfo(HWND hwnd, PMINMAXINFO pMinMaxInfo) 
{
	// Return minimum size of dialog box
//	g_UILayout.HandleMinMax(pMinMaxInfo);
}


INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		chHANDLE_DLGMSG(hwnd, WM_INITDIALOG,    Dlg_OnInitDialog);
		chHANDLE_DLGMSG(hwnd, WM_DESTROY,       Dlg_OnDestroy);
		chHANDLE_DLGMSG(hwnd, WM_COMMAND,       Dlg_OnCommand);
		chHANDLE_DLGMSG(hwnd, WM_SIZE,          Dlg_OnSize);
		chHANDLE_DLGMSG(hwnd, WM_GETMINMAXINFO, Dlg_OnGetMinMaxInfo);
	}
	return(FALSE);
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) 
{
	const WCHAR *mystr = L"My private string";
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)mystr);
	return(0);
}

