/*------------------------------------------------------------
wmShowWindow2.cpp skeleton program 
-- Displays "Hello, WindowsX!" in client area
				 
   Compile it with command line in Visual C++ 2010+:
   
cl /c /Od /MT /Zi /D_DEBUG /D_UNICODE /DUNICODE wmShowWindow2.cpp
rc wmShowWindow2.rc
link /debug wmShowWindow2.obj wmShowWindow2.res kernel32.lib user32.lib gdi32.lib

   Then we can load it into Visual C++ debugger.
  ------------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "../utils.h"

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PTSTR szCmdLine, int iCmdShow)
{
	(void)hPrevInstance; (void)szCmdLine; 
	static TCHAR szAppName[] = TEXT ("wmShowWindow2") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

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
		TEXT ("wmShowWindow2"),        // window caption
		WS_OVERLAPPEDWINDOW,           // window style
		20,              // initial x position
		20,              // initial y position
		400,             // initial x size
		200,             // initial y size
		NULL,            // parent window handle
		NULL,            // window menu handle
		hInstance,       // program instance handle
		NULL) ;          // creation parameters
	 
	SendMessage(hwnd, WM_SETICON, TRUE, (LPARAM)LoadIcon(hInstance,	MAKEINTRESOURCE(1)));

	vaDbgTs(_T(">> ShowWindow(hwnd=0x%X)"), (UINT)hwnd);
	ShowWindow (hwnd, iCmdShow) ;
	vaDbgTs(_T("<< ShowWindow()"));

	vaDbgTs(_T(">> UpdateWindow()"));
	UpdateWindow (hwnd) ;
	vaDbgTs(_T("<< UpdateWindow()"));
	
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	return msg.wParam; // the value N told by PostQuitMessage(N);
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
	DrawText (hdc, 
		TEXT ("Shows my WM_SHOWWINDOW messages.\r\n")
		TEXT ("Use DbgView.exe to see my output.")
		, -1, &rect,
		DT_CENTER | DT_VCENTER) ;

	EndPaint (hwnd, &ps) ;
}

void Cls_OnDestroy(HWND hwnd)
{
	PostQuitMessage(44);
}

void Cls_OnShowWindow(HWND hwnd, BOOL fShow, UINT status)
{
	vaDbgTs(_T("-- in WM_SHOWWINDOW, fShow=%d , status=%d"), fShow, status);
}

void Cls_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	vaDbgTs(_T("-- in WM_SIZE, cx=%d , cy=%d"), cx, cy);
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{{
		HANDLE_MSG(hwnd, WM_CREATE, Cls_OnCreate);
		HANDLE_MSG(hwnd, WM_PAINT, Cls_OnPaint);
		HANDLE_MSG(hwnd, WM_DESTROY, Cls_OnDestroy);

		HANDLE_MSG(hwnd, WM_SHOWWINDOW, Cls_OnShowWindow);
		HANDLE_MSG(hwnd, WM_SIZE, Cls_OnSize);
	}}
	
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
