/*------------------------------------------------------------
Weird: assert(0) does NOT bring up the Assertion Fail dialog, but overflow the stack.
  ------------------------------------------------------------*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
void vaDbg(const TCHAR *fmt, ...);
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PTSTR szCmdLine, int iCmdShow)
{
	(void)hPrevInstance; (void)szCmdLine; 
	static TCHAR szAppName[] = TEXT ("WM_PAINT_recurse_bomb") ;
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
		TEXT ("The WM_PAINT_recurse_bomb Program"), // window caption
		WS_OVERLAPPEDWINDOW,           // window style
		20,              // initial x position
		20,              // initial y position
		400,             // initial x size
		200,             // initial y size
		NULL,            // parent window handle
		NULL,            // window menu handle
		hInstance,       // program instance handle
		NULL) ;          // creation parameters
	 
	SendMessage(hwnd, WM_SETICON, TRUE, (LPARAM)LoadIcon(hInstance,	TEXT("MYPROGRAM")));

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;
	
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	return 0;
}

#define TIMER_ID 1
int g_bombflag = 0;

void DoTimer(HWND hwnd)
{
	g_bombflag = 1;
	KillTimer(hwnd, TIMER_ID);
	InvalidateRect(hwnd, NULL, TRUE);
}

void DoPaint(HWND hwnd)
{
	HDC hdc = NULL;
	PAINTSTRUCT ps = {};
	RECT rect = {};

	if(g_bombflag==1)
	{
		// This assert(), written *before* BeginPaint(), will trigger 
		// *recursive* calling of WM_PAINT, thus crash the program.
		assert(g_bombflag==0);

		// Using MessageBox() is also bad:
		// MessageBox(hwnd, _T("Msgbox inside WM_PAINT."), _T("title"), MB_OK);
	}

	hdc = BeginPaint(hwnd, &ps);

	GetClientRect (hwnd, &rect) ;          
	Ellipse(hdc, 0,0, rect.right, rect.bottom);
	DrawText (hdc, TEXT ("Hello, Windows!"), -1, &rect,
		DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;

	EndPaint(hwnd, &ps) ;
	return ;
}  


LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{{
	case WM_CREATE:
		{
			SetTimer(hwnd, TIMER_ID, 1000, NULL);
			return 0 ;
		}

	case WM_PAINT:
		DoPaint(hwnd);
		return 0;
	case WM_TIMER:
		DoTimer(hwnd);
		return 0;
	case WM_DESTROY:
		PostQuitMessage (0) ;
		return 0 ;
	}}
	
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}

void vaDbg(const TCHAR *fmt, ...)
{
	static int count = 0;
	TCHAR buf[1000] = {0};

	_sntprintf_s(buf, ARRAYSIZE(buf)-3, _TRUNCATE, TEXT("[%d] "), ++count); // prefix seq

	int prefixlen = (int)_tcslen(buf);

	va_list args;
	va_start(args, fmt);

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);
	prefixlen = (int)_tcslen(buf);
	_tcsncpy_s(buf+prefixlen, 2, TEXT("\r\n"), _TRUNCATE); // add trailing \r\n

	va_end(args);

	OutputDebugString(buf);
}
