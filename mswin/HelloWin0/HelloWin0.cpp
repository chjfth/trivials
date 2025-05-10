// This program boldly uses DefWindowProc() as WndProc.
// The message loop code should be adjusted so that [X]closing the window can
// tell the process to quit.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include <mswin/winuser.itc.h>

#include "..\utils.h"

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PTSTR szCmdLine, int nCmdShow)
{
	(void)hPrevInstance; (void)szCmdLine; 
	static TCHAR szAppName[] = TEXT ("HelloWin0") ;
	HWND         hwnd = NULL;
	MSG          msg = {};
	WNDCLASS     wndclass = {};

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = DefWindowProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (hInstance, TEXT("MYPROGRAM")) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	RegisterClass (&wndclass);
	 
	hwnd = CreateWindow (szAppName,    // window class name
		TEXT ("The HelloWin0 Program"), // window caption
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
	
	DWORD waitResult = 0;
	DWORD chkinteral = 2000; // millisec
	bool is_quit = false;

	while (true) 
	{
		waitResult = MsgWaitForMultipleObjects(
			0, NULL, FALSE, 
			chkinteral,
			QS_ALLINPUT);

		if(!IsWindow(hwnd))
		{
			// The window has been destroyed, but we are curious to see
			// whether we can peek some MSG.
			is_quit = true; 

			vaDbgTs(_T("IsWindow() reports FALSE, now waitResult=%d."), waitResult);
		}

		if (waitResult == WAIT_OBJECT_0) {
			// Messages are available
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {

				vaDbgTs(_T("Peeked message: %s"), ITCSv(msg.message, itc::WM_xxx));

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else if (waitResult == WAIT_TIMEOUT) {
			if(is_quit)
			{
				vaDbgTs(_T("IsWindow() reports FALSE due to MsgWaitForMultipleObjects() timeout."));
			}
		}
		else {
			// Error occurred
			break;
		}

		if(is_quit) {
			vaDbgTs(_T("is_quit==true, break out of message loop."));
			break;
		}
	}

	return (int)msg.wParam; // the value N told by PostQuitMessage(N);
}

