#define WIN32_LEAN_AND_MEAN
#include <WindowsX.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "CmnHdr-Jeffrey.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "utils.h"

///////////////////////////////////////////////////////////////////////////////

struct DlgPrivate_st
{
	const WCHAR *mystr;
};

void Draw_CtlBorder(HWND hdlg, int idCtl, COLORREF colorref)
{
	HWND hctl = GetDlgItem(hdlg, idCtl);
	HDC hdc = GetDC(hctl);

	RECT r = { 0 };
	GetClientRect(hctl, &r);

	HPEN hPen = CreatePen(PS_SOLID, 1, colorref);
	SelectObject(hdc, hPen);
	Rectangle(hdc, r.left, r.top, r.right, r.bottom);

	DeleteObject(GetStockObject(BLACK_PEN));
	ReleaseDC(hctl, hdc);
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	chSETDLGICONS(hdlg, IDI_WINMAIN);

	DlgPrivate_st *pr = new DlgPrivate_st;
	pr->mystr = (const WCHAR*)lParam;

	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)pr);

	SetDlgItemText(hdlg, IDC_LBL_HIDDEN, L"");
	const WCHAR *text_banner = 
L"Click a button to launch a thread.\r\n"
L"\r\n"
L" >> For first button, the thread will call UI function directly;\r\n"
L" >> For second button, marshaling call is carried out.\r\n"
L"\r\n"
L"(TODO) Run this program with VS debugger attached, the first button will cause "
L"'Cross-thread operation not valid' exception. and the second will be fine.\r\n"
	;
	SetDlgItemText(hdlg, IDC_BANNER_TEXT, text_banner);

	SetDlgItemText(hdlg, IDC_COLOR_BLOCK, TEXT(""));
	Draw_CtlBorder(hdlg, IDC_COLOR_BLOCK, RGB(0, 222, 0));

	PostMessage(hdlg, WM_APP, 0, 0);
	
	return(TRUE);
}

void Dlg_OnDestroy(HWND hwnd)
{
	DlgPrivate_st *pr = (DlgPrivate_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	// Destroy all resources allocated back in Dlg_OnInitDialog().
	delete pr;
}

void Paint_ColorBlock(HWND hdlg, bool isRightSide, COLORREF colorref)
{
	HWND hctl = GetDlgItem(hdlg, IDC_COLOR_BLOCK);

	HDC hdc = GetDC(hctl);

	RECT rColorBlock = { 0 };
	GetClientRect(hctl, &rColorBlock);

	RECT rLeft = rColorBlock;
	rLeft.right = rColorBlock.right / 2;
	RECT rRight = rColorBlock;
	rRight.left = rColorBlock.right / 2;

	HBRUSH hBrush = CreateSolidBrush(colorref);
	FillRect(hdc, isRightSide ? &rRight : &rLeft, hBrush);

	DeleteObject(GetStockObject(BLACK_BRUSH));
	ReleaseDC(hctl, hdc);
}

int thread_DirectUI(void *param)
{
	HWND hdlg = (HWND)param;
	Paint_ColorBlock(hdlg, false, RGB(255, 0, 0));

//	SetDlgItemText(hdlg, IDC_LBL_MESSAGE, L"Direct text from worker thread.");
	return 0;
}

int thread_extraGUI(void *param)
{
//	Checker2_RunGUIThread();
	return 0;
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
//	DlgPrivate_st *pr = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDOK:
	case IDCANCEL:
		EndDialog(hdlg, id);
		break;

	case ID_BTN_UIDIRECT:
	{
		HANDLE hThread = winCreateThread(thread_DirectUI, hdlg);

		SetDlgItemText(hdlg, IDC_LBL_MESSAGE, L"Created the DirectUI thread. Waiting 3 seconds for thread end...");
		DWORD thread_ret = winWaitThreadEnd(hThread, 3000); (void)thread_ret;

		if (thread_ret == 0) {
			SetDlgItemText(hdlg, IDC_LBL_MESSAGE, L"DirectUI thread done.");
		}
		else {
			SetDlgItemText(hdlg, IDC_LBL_MESSAGE, L"DirectUI thread not finished yet.");
			Sleep(1000);
		}

		(void)hThread; 
		break;
	}
	case ID_BTN_UIMARSHAL:
	{
Draw_CtlBorder(hdlg, IDC_COLOR_BLOCK, RGB(0, 222, 0));			
		Paint_ColorBlock(hdlg, true, RGB(0,0,255)); // temp
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


INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		chHANDLE_DLGMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		chHANDLE_DLGMSG(hdlg, WM_DESTROY,       Dlg_OnDestroy);
		chHANDLE_DLGMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
		chHANDLE_DLGMSG(hdlg, WM_SIZE,          Dlg_OnSize);
		chHANDLE_DLGMSG(hdlg, WM_GETMINMAXINFO, Dlg_OnGetMinMaxInfo);

	case WM_APP:
		MessageBox(hdlg,
			IsWindowVisible(hdlg) ? TEXT("parent Visible") : TEXT("parent Not Visible"),
			TEXT("case WM_APP:"),
			MB_OK);
		break;
	}
	return(FALSE);
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) 
{
	const WCHAR *mystr = L"My private string";
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)mystr);
	return(0);
}

