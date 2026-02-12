#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#define EXE_VERSION "1.1"

#include "utils.h"

#include <mswin/winuser.itc.h>
using namespace itc;

#define HT_NOT_FAKE (-4)
int g_HTxxx_to_fake = HT_NOT_FAKE; 
// -- Can be overridden by command line param
// You can pass HTLEFT , HTCAPTION etc as argument.

void Prepare_CmdParams(const TCHAR *pszargs)
{
	if(pszargs==NULL || *pszargs=='\0')
		return;

	g_HTxxx_to_fake = HTxxx_HitTest.OneNameToVal(pszargs);
}


LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PTSTR szCmdLine, int nCmdShow)
{
	(void)hPrevInstance; (void)szCmdLine; 
	static TCHAR szAppName[] = TEXT ("tWinCursor1") ;
	HWND         hwnd = NULL;
	MSG          msg = {};
	WNDCLASS     wndclass = {};

	Prepare_CmdParams(szCmdLine);

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (hInstance, MAKEINTRESOURCE(1)) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	RegisterClass (&wndclass);
	 
	hwnd = CreateWindow (szAppName,    // window class name
		TEXT ("The tWinCursor1 Program"), // window caption
		WS_OVERLAPPEDWINDOW,           // window style
		CW_USEDEFAULT,   // initial x position
		CW_USEDEFAULT,   // initial y position
		400,             // initial x size
		200,             // initial y size
		NULL,            // parent window handle
		NULL,            // window menu handle
		hInstance,       // program instance handle
		NULL) ;          // creation parameters
	 
	ShowWindow (hwnd, nCmdShow) ;
	UpdateWindow (hwnd) ;
	
	static int s_count = 0;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		itc::String wmstr = ITCSv(msg.message, WM_xxx);

		vaDbgTs(_T("#%d - MSG: %s >>>" ), ++s_count, wmstr.get());

		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;

		vaDbgTs(_T("#%d - MSG: %s <<<" ), s_count, wmstr.get());
	}

	return (int)msg.wParam; // the value N told by PostQuitMessage(N);
}

BOOL Cls_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	vaSetWindowText(hwnd, _T("tWinCursor1 v%s"), _T(EXE_VERSION));
	return TRUE; // success, go on creation
}

void Cls_OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps = {};
	RECT        rect ;
	HDC hdc = BeginPaint (hwnd, &ps) ;

	GetClientRect (hwnd, &rect) ;          
	Ellipse(hdc, 0,0, rect.right, rect.bottom);

	TCHAR szFakeinfo[100] = {};
	const TCHAR *ptext = _T("Hello, WM_SETCURSOR!");

	if(g_HTxxx_to_fake!=HT_NOT_FAKE)
	{
		_sntprintf_s(szFakeinfo, _TRUNCATE, _T("Will fake WM_NCHITTEST with %s."),
			ITCS(g_HTxxx_to_fake, HTxxx_HitTest));

		ptext = szFakeinfo;
	}

	DrawText (hdc, ptext, -1, &rect,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;

	EndPaint (hwnd, &ps) ;
}

void Cls_OnDestroy(HWND hwnd)
{
	vaDbgTs(_T("====WM_DESTROY===="));
	PostQuitMessage(44);
}

void Cls_OnClose(HWND hwnd)
{
	vaDbgTs(_T("====WM_CLOSE===="));
	FORWARD_WM_CLOSE(hwnd, DefWindowProc);
}


UINT Cls_OnNCHitTest(HWND hwnd, int x, int y)
{
	UINT htcode = FORWARD_WM_NCHITTEST(hwnd, x, y, DefWindowProc);

	if(g_HTxxx_to_fake==HT_NOT_FAKE)
	{
		vaDbgTs(_T("WM_NCHITTEST [%d,%d] returns HT=%s"), x, y, ITCSv(htcode, HTxxx_HitTest));
		return htcode;
	}
	else
	{
		vaDbgTs(_T("WM_NCHITTEST [%d,%d] returns FAKED %s"), x, y, 
			ITCSv(g_HTxxx_to_fake, HTxxx_HitTest));
		return g_HTxxx_to_fake;
	}
}

BOOL Cls_OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT mousemsg)
{
	POINT pt = {};
	BOOL succ = GetCursorPos(&pt);
	assert(succ);

	if(hwnd==hwndCursor)
	{
		vaDbgTs(_T("WM_SETCURSOR: HT=%s , mousemsg=%s [%d,%d]"), 
			ITCSv(codeHitTest, HTxxx_HitTest), 
			ITCSv(mousemsg, WM_xxx), 
			pt.x, pt.y);
	}
	else
	{
		// PendingQ: When can I see this?
		vaDbgTs(_T("WM_SETCURSOR[0x%08X]: HT=%s , mousemsg=%s [%d,%d]}"), 
			hwndCursor, 
			ITCSv(codeHitTest, HTxxx_HitTest), 
			ITCSv(mousemsg, WM_xxx), 
			pt.x, pt.y);
	}

	return FORWARD_WM_SETCURSOR(hwnd, hwndCursor, codeHitTest, mousemsg, DefWindowProc);
}

void Cls_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	vaDbgTs(_T("WM_LBUTTONDOWN keyFlags=0x%X"), keyFlags);

//	SetCursor(LoadCursor(NULL, IDC_SIZEALL));
}

void Cls_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	vaDbgTs(_T("WM_LBUTTONUP"));

//	SetCursor(LoadCursor(NULL, IDC_IBEAM));
}


void Cls_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	vaDbgTs(_T("WM_RBUTTONDOWN"));
//	SetCursor(LoadCursor(NULL, IDC_SIZEALL));

	HWND hwndDesk = GetDesktopWindow();
	printf("hwndDesktop = 0x%X\n", hwndDesk);

//	Sleep(1000);
//	SendMessage(hwndDesk, WM_SYSCOMMAND, SC_MONITORPOWER, (LPARAM)2);
}


void Cls_OnTimer(HWND hwnd, UINT id)
{
	int fMouse = GetSystemMetrics (SM_MOUSEPRESENT);
	vaDbgTs(_T("[T1] SM_MOUSEPRESENT: %d"), fMouse);
}

void Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	if(fDown)
	{
		Sleep(500);
		vaDbgTs(_T("KEYDOWN repeat: %d"), cRepeat);
	}
}

void Cls_OnNCLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT codeHitTest)
{
	if(!fDoubleClick)
	{
		vaDbgTs(_T("Cls_OnNCLButtonDown HT=%s [%d,%d]"), 
			ITCSv(codeHitTest, HTxxx_HitTest), 
			x,y);
	}
	else
	{
		vaDbgTs(_T("Cls_OnNCLButtonDown HT=%s [%d,%d] (DB-click)"), 
			ITCSv(codeHitTest, HTxxx_HitTest), 
			x,y);
	}
	FORWARD_WM_NCLBUTTONDOWN(hwnd, fDoubleClick, x, y, codeHitTest, DefWindowProc);
}

void Cls_OnNCLButtonUp(HWND hwnd, int x, int y, UINT codeHitTest)
{
	vaDbgTs(_T("Cls_OnNCLButtonUp HT=%s [%d,%d]"), 
		ITCSv(codeHitTest, HTxxx_HitTest), 
		x,y);
}

void Cls_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
	vaDbgTs(_T("WM_MOUSEMOVE [%d,%d]"), x, y);
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{{
		HANDLE_MSG(hwnd, WM_CREATE, Cls_OnCreate);
		HANDLE_MSG(hwnd, WM_PAINT, Cls_OnPaint);
		HANDLE_MSG(hwnd, WM_DESTROY, Cls_OnDestroy);
		HANDLE_MSG(hwnd, WM_CLOSE, Cls_OnClose);

		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, Cls_OnLButtonDown);
		HANDLE_MSG(hwnd, WM_LBUTTONUP, Cls_OnLButtonUp);

		HANDLE_MSG(hwnd, WM_NCHITTEST, Cls_OnNCHitTest); // highlight
		HANDLE_MSG(hwnd, WM_SETCURSOR, Cls_OnSetCursor);

		HANDLE_MSG(hwnd, WM_RBUTTONDOWN, Cls_OnRButtonDown);

		HANDLE_MSG(hwnd, WM_TIMER, Cls_OnTimer);
		HANDLE_MSG(hwnd, WM_KEYDOWN, Cls_OnKey);

		HANDLE_MSG(hwnd, WM_NCLBUTTONDOWN, Cls_OnNCLButtonDown);
		HANDLE_MSG(hwnd, WM_NCLBUTTONDBLCLK, Cls_OnNCLButtonDown);
		HANDLE_MSG(hwnd, WM_NCLBUTTONUP, Cls_OnNCLButtonUp);

		HANDLE_MSG(hwnd, WM_MOUSEMOVE, Cls_OnMouseMove);
	}}
	
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
