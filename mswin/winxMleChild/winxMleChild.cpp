#define _WIN32_WINNT 0x0500 // NONCLIENTMETRICS matters, only need WinXP members

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "../utils.h"

bool g_insideGetMessage = false;

const TCHAR *InsideGetMessageMark()
{
	return g_insideGetMessage ? _T("-") : _T("%");
}

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PTSTR szCmdLine, int nCmdShow)
{
	(void)hPrevInstance; (void)szCmdLine; 
	static TCHAR szAppName[] = TEXT ("winxMleChild") ;
	HWND         hwnd = NULL;
	MSG          msg = {};
	WNDCLASS     wndclass = {};

	NONCLIENTMETRICS ncmetrics = {sizeof(ncmetrics)};
	BOOL succ = SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncmetrics), (void*)&ncmetrics, 0);
	int iBorder = ncmetrics.iBorderWidth;
	int iCaptionHeight = ncmetrics.iCaptionHeight;

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
		TEXT ("winxMleChild v1.0"), // window caption
		WS_OVERLAPPEDWINDOW,           // window style
		50,   // initial x position
		50,   // initial y position
		300 + iBorder*2,                  // initial x size
		200 + iBorder*2 + iCaptionHeight, // initial y size
		NULL,            // parent window handle
		NULL,            // window menu handle
		hInstance,       // program instance handle
		NULL) ;          // creation parameters
	 
	ShowWindow (hwnd, nCmdShow) ;
	UpdateWindow (hwnd) ;
	
	for(;;) 
	{
		g_insideGetMessage = true;
		BOOL gotmsg = GetMessage (&msg, NULL, 0, 0);
		g_insideGetMessage = false;
			
		if(!gotmsg)
			break;

		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	return (int)msg.wParam; // the value N told by PostQuitMessage(N);
}

HWND g_hedit;
WNDPROC g_prevEditProc;

LRESULT CALLBACK
MyEdit_SubclassProc(HWND hedit, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg==WM_MOUSEACTIVATE)
	{
		RECT rc = {};
		GetClientRect(hedit, &rc);
		int midx = rc.right / 2;
		int midy = rc.bottom / 2;

		POINT &pt = *(POINT*)&rc;
		GetCursorPos(&pt);
		ScreenToClient(hedit, &pt);

		vaDbgTs(_T("%s editbox.WM_MOUSEACTIVATE>> mousemsg=%d, HTcode=%d ; clix=%d, cliy=%d"), 
			InsideGetMessageMark(),
			HIWORD(lParam), LOWORD(lParam),    pt.x, pt.y);
	}

	LRESULT lret = CallWindowProc(g_prevEditProc, hedit, msg, wParam, lParam);

	if(msg==WM_MOUSEACTIVATE)
	{
		vaDbgTs(_T("%s editbox.WM_MOUSEACTIVATE<< returning %d"), 
			InsideGetMessageMark(),
			(int)lret);
	}

	return lret;
}

BOOL Cls_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	g_hedit = CreateWindow ( _T("edit"),    // window class name
		TEXT ("i'm editbox MLE\r\n\r\n# # WOW # #\r\n\r\n@ @ HAHA @ @"), // window caption
		WS_CHILD|WS_VISIBLE|WS_VSCROLL|ES_MULTILINE|ES_WANTRETURN|WS_BORDER|WS_TABSTOP, 
		50,   // initial x position
		50,   // initial y position
		200,             // initial x size
		100,             // initial y size
		hwnd,            // parent window handle
		HMENU(1),        // window menu handle
		GetWindowInstance(hwnd), // program instance handle
		NULL) ;          // creation parameters

    g_prevEditProc = SubclassWindow(g_hedit, MyEdit_SubclassProc);

	return TRUE; // success, go on creation
}

void Cls_OnPaint(HWND hwnd)
{
	PAINTSTRUCT ps = {};
	RECT        rect ;
	HDC hdc = BeginPaint (hwnd, &ps) ;

	GetClientRect (hwnd, &rect) ;

	// Draw the quadrants
	MoveToEx(hdc, rect.right/2, 0, NULL);
	LineTo(hdc, rect.right/2, rect.bottom);
	MoveToEx(hdc, 0, rect.bottom/2, NULL);
	LineTo(hdc, rect.right, rect.bottom/2);

	// Mark Zone A,B,C,D 

	RECT trc = {}; // text rectangle
	const int tw=15, th=20;
	const int drawflags = DT_SINGLELINE | DT_CENTER | DT_VCENTER;
	
	SetRect(&trc, 0,0, tw,th);
	DrawText (hdc, _T("A"), -1, &trc, drawflags);

	SetRect(&trc, rect.right-tw, 0,  rect.right, th);
	DrawText (hdc, _T("B"), -1, &trc, drawflags);

	SetRect(&trc, 0, rect.bottom-th, tw, rect.bottom);
	DrawText (hdc, _T("C"), -1, &trc, drawflags);

	SetRect(&trc, rect.right-tw, rect.bottom-th, rect.right, rect.bottom);
	DrawText (hdc, _T("D"), -1, &trc, drawflags);

	EndPaint (hwnd, &ps) ;
}

void Cls_OnDestroy(HWND hwnd)
{
	PostQuitMessage(44);
}

void Cls_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	vaDbgTs(_T("%s mainwnd left-click down: x=%d, y=%d"), 
		InsideGetMessageMark(),
		x, y);
}

void Cls_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
	vaDbgTs(_T("%s mainwnd left-click up  : x=%d, y=%d"), 
		InsideGetMessageMark(),
		x, y);
}

void Cls_OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	vaDbgTs(_T("%s mainwnd right-click down: x=%d, y=%d"), 
		InsideGetMessageMark(),
		x, y);
}

void Cls_OnRButtonUp(HWND hwnd, int x, int y, UINT flags)
{
	vaDbgTs(_T("%s mainwnd right-click up  : x=%d, y=%d"), 
		InsideGetMessageMark(),
		x, y);
}

int Cls_OnMouseActivate(HWND hwnd, HWND hwndTopLevel, UINT codeHitTest, UINT mousemsg)
{
	RECT rc = {};
	GetClientRect(hwnd, &rc);
	int midx = rc.right / 2;
	int midy = rc.bottom / 2;

	POINT &pt = *(POINT*)&rc;
	GetCursorPos(&pt);
	ScreenToClient(hwnd, &pt);
	
	int ret = 0;
	const TCHAR *szret = nullptr;

	if(pt.x<midx && pt.y<midy) // left-top
	{
		ret = MA_ACTIVATE;
		szret = _T("MA_ACTIVATE");
	}
	else if(pt.x>=midx && pt.y<midy) // right-top
	{
		ret = MA_ACTIVATEANDEAT;
		szret = _T("MA_ACTIVATEANDEAT");
	}
	else if(pt.x<midx && pt.y>=midy) // left-bottom
	{
		ret = MA_NOACTIVATE;
		szret = _T("MA_NOACTIVATE");
	}
	else if(pt.x>=midx && pt.y>=midy) // right-bottom
	{
		ret = MA_NOACTIVATEANDEAT;
		szret = _T("MA_NOACTIVATEANDEAT");
	}

	vaDbgTs(_T("%s mainwnd.WM_MOUSEACTIVATE : mousemsg=%d, HTcode=%d ; clix=%d, cliy=%d ; ret %d(%s)"), 
		InsideGetMessageMark(),
		mousemsg, codeHitTest, pt.x, pt.y,
		ret, szret);

	return ret;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{{
		HANDLE_MSG(hwnd, WM_CREATE, Cls_OnCreate);
		HANDLE_MSG(hwnd, WM_PAINT, Cls_OnPaint);
		HANDLE_MSG(hwnd, WM_DESTROY, Cls_OnDestroy);

		HANDLE_MSG(hwnd, WM_LBUTTONDOWN, Cls_OnLButtonDown);
		HANDLE_MSG(hwnd, WM_LBUTTONUP  , Cls_OnLButtonUp);
		HANDLE_MSG(hwnd, WM_RBUTTONDOWN, Cls_OnRButtonDown);
		HANDLE_MSG(hwnd, WM_RBUTTONUP  , Cls_OnRButtonUp);

		HANDLE_MSG(hwnd, WM_MOUSEACTIVATE, Cls_OnMouseActivate);
	}}
	
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
