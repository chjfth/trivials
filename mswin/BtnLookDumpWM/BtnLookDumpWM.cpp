/*------------------------------------------------------------
BtnLookDumpWM.cpp 
Based on PRWIN5 Chap09 BtnLook.c, we dump all window-messages we see to debug channel.
				 
   Compile it with command line in Visual C++ 2010+:
   
cl /c /Od /MT /Zi /D_DEBUG /D_UNICODE /DUNICODE BtnLookDumpWM.cpp
rc BtnLookDumpWM.rc
link /debug BtnLookDumpWM.obj BtnLookDumpWM.res kernel32.lib user32.lib gdi32.lib

   Then we can load it into Visual C++ debugger.
  ------------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <CommCtrl.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "resource.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define CHHI_ALL_IMPL
#include <snTprintf.h>
#include <mswin/utils_env.h>
#include <mswin/utils_wingui.h>
#include <mswin/WinUser.itc.h>

#define VER_STR "1.2"

int g_nestlv = 0; // WM_xxx nested level
int g_msgcount = 0;

const TCHAR* get_indents(int nestlv)
{
	static TCHAR s_indents[40];

	int i;
	for(i=0; i<nestlv-1 && i<ARRAYSIZE(s_indents)-1; i++)
		s_indents[i] = ' ';

	s_indents[i] = '\0';
	return s_indents;
}

class AutoNestCount
{
public:
	AutoNestCount(int *pNestLv, UINT msg) : m_pNestLv(pNestLv), m_msg(msg)
	{ 
		(*m_pNestLv)++; 

		vaDbgTs(_T("[#%-3d]%s[*%d] >>> Enter %s"), 
			g_msgcount, get_indents(*m_pNestLv), *m_pNestLv, // [#%d]%s[*%d]
			ITCSnv(m_msg, itc::WM_xxx));
	}
	~AutoNestCount()
	{ 
		vaDbgTs(_T("[#%-3d]%s[*%d] <<< Leave %s"), 
			g_msgcount, get_indents(*m_pNestLv), *m_pNestLv, // [#%d]%s[*%d]
			ITCSnv(m_msg, itc::WM_xxx));

		(*m_pNestLv)--; 
	}
private:
	int *m_pNestLv;
	UINT m_msg;
};

struct
{
	int     iStyle ;
	TCHAR * szText ;
} 
button[] =
{
	BS_PUSHBUTTON,      TEXT ("PUSHBUTTON"),
	BS_DEFPUSHBUTTON,   TEXT ("DEFPUSHBUTTON"),
	BS_CHECKBOX,        TEXT ("CHECKBOX"), 
	BS_AUTOCHECKBOX,    TEXT ("AUTOCHECKBOX"),
	BS_RADIOBUTTON,     TEXT ("RADIOBUTTON"),
	BS_3STATE,          TEXT ("3STATE"),
	BS_AUTO3STATE,      TEXT ("AUTO3STATE"),
	BS_GROUPBOX,        TEXT ("GROUPBOX"),
	BS_AUTORADIOBUTTON, TEXT ("AUTORADIO"),
	BS_OWNERDRAW,       TEXT ("OWNERDRAW")
};

#define NUM  ARRAYSIZE(button)

#define TIMER_ID 1

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PTSTR szCmdLine, int nCmdShow)
{
	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	static TCHAR szAppName[] = TEXT ("BtnLook") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(1));
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = MAKEINTRESOURCE(1);
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"),
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	const int client_width = util_SimpleSysDpiScale(540);
	const int client_height = util_SimpleSysDpiScale(410);

	hwnd = CreateWindow (szAppName, 
		TEXT("BtnLook"), // will modify later
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		client_width, client_height,
		NULL, NULL, hInstance, NULL);
	Set_ClientAreaSize(hwnd, client_width, client_height);

	SetWindowText(hwnd, _T("BtnLook dumps window-messages to debug-channel ") _T(VER_STR));

	ShowWindow (hwnd, nCmdShow) ;
	UpdateWindow (hwnd) ;

	HWND hwnd_mytop = hwnd;
	vaDbgTs(_T("My main window HWND=0x%X"), (UINT)hwnd_mytop);

	while (1)
	{
		vaDbgTs(_T("GetMessage() >>>"));
		
		BOOL succ = GetMessage (&msg, NULL, 0, 0);

		// Check msg.hwnd's traits
		HWND hwnd_root = GetAncestor(msg.hwnd, GA_ROOT);
		const TCHAR *trait = _T("");
		if(msg.hwnd==hwnd_mytop)
			trait = _T("top");
		else if(hwnd_root==hwnd_mytop)
			trait = _T("child");
		else
		{
			// Other top-level window, it could be a class="MSCTFIME UI"
			trait = _T("other");
		}

		vaDbgTs(_T("GetMessage() <<< got %s (hwnd=0x%X) %s"), 
			ITCSnv(msg.message, itc::WM_xxx), 
			(UINT)msg.hwnd, // (hwnd=0x%X)
			trait // %s
			);

		if(!succ)
			break;

		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	return (int)msg.wParam ;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	g_msgcount++;
	AutoNestCount nestcount(&g_nestlv, message);

	static HWND  hwndButton[NUM] ;
	static RECT  rect ;
	static TCHAR szTop[]    = 
		             TEXT ("message            wParam       lParam"),
		szUnd[]    = TEXT ("_______            ______       ______"),
		szFormat[] = TEXT ("%-16s%04X-%04X    %04X-%04X"),
		szBuffer[50] ;
	static int   cxChar, cyChar ;
	HDC          hdc ;
	PAINTSTRUCT  ps ;
	int          i ;

	switch (message)
	{
	case WM_CREATE :

		cxChar = LOWORD (GetDialogBaseUnits()) ;
		cyChar = HIWORD (GetDialogBaseUnits()) ;

		for (i = 0 ; i < NUM ; i++)
		{
			hwndButton[i] = CreateWindow ( TEXT("button"), 
				button[i].szText,
				WS_CHILD | WS_VISIBLE | button[i].iStyle,
				cxChar, cyChar * (1 + 2 * i),
				20 * cxChar, 7 * cyChar / 4,
				hwnd, 
				(HMENU) i,
				((LPCREATESTRUCT) lParam)->hInstance, 
				NULL) ;
		}
		return 0 ;

	case WM_SIZE :
		rect.left   = 24 * cxChar ;
		rect.top    =  2 * cyChar ;
		rect.right  = LOWORD (lParam) ;
		rect.bottom = HIWORD (lParam) ;
		return 0 ;

	case WM_PAINT :
		InvalidateRect (hwnd, &rect, TRUE) ;

		hdc = BeginPaint (hwnd, &ps) ;
		SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;
		SetBkMode (hdc, TRANSPARENT) ;

		TextOut (hdc, 24 * cxChar, cyChar, szTop, lstrlen (szTop)) ;
		TextOut (hdc, 24 * cxChar, cyChar, szUnd, lstrlen (szUnd)) ;
		EndPaint (hwnd, &ps) ;
		return 0 ;

	case WM_DRAWITEM :
	case WM_COMMAND :

		ScrollWindow (hwnd, 0, -cyChar, &rect, &rect) ;

		hdc = GetDC (hwnd) ;
		SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;

		TextOut (hdc, 24 * cxChar, cyChar * (rect.bottom / cyChar - 1),
			szBuffer,
			wsprintf (szBuffer, szFormat,
				message == WM_DRAWITEM ? TEXT ("WM_DRAWITEM") : 
				TEXT ("WM_COMMAND"),
				HIWORD (wParam), LOWORD (wParam),
				HIWORD (lParam), LOWORD (lParam))
		);

		ReleaseDC (hwnd, hdc) ;

		if(LOWORD(wParam)==IDM_APP_ABOUT)
		{
			DialogBox(GetWindowInstance(hwnd), TEXT("AboutBox"), hwnd, AboutDlgProc);
		}
		else if(LOWORD(wParam)==IDM_APP_ABOUT_DELAY)
		{
			SetTimer(hwnd, TIMER_ID, 2000, NULL);
		}

		ValidateRect (hwnd, &rect) ;
		break ;

	case WM_TIMER:
		if(wParam==TIMER_ID)
		{
			KillTimer(hwnd, TIMER_ID);
			DialogBox(GetWindowInstance(hwnd), TEXT("AboutBox"), hwnd, AboutDlgProc);
		}
		return 0;

	case WM_DESTROY :
		PostQuitMessage (0) ;
		return 0 ;
	}

	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

INT_PTR CALLBACK AboutDlgProc(HWND hDlg, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}
