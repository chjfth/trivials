/*------------------------------------------------------------
   HELLOWIN.C -- Displays "Hello, Windows 98!" in client area
				 (c) Charles Petzold, 1998
				 
   HelloWin.cpp -- A bit modification for easier debugging.
				-- by Jimm Chen
   Compile it with command line in Visual C++ 2010+:
   
cl /Od /MT /D_DEBUG /D_UNICODE /DUNICODE HelloWin.cpp /link kernel32.lib user32.lib gdi32.lib 

   Then we can load it into Visual C++ debugger.
  ------------------------------------------------------------*/

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

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

LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PSTR szCmdLine, int iCmdShow)
{
	(void)hPrevInstance; (void)szCmdLine; 
	static TCHAR szAppName[] = TEXT ("HelloWin") ;
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
	 
	hwnd = CreateWindow (szAppName,                  // window class name
		TEXT ("The Hello Program"), // window caption
		WS_OVERLAPPEDWINDOW,        // window style
		20,              // initial x position
		20,              // initial y position
		400,             // initial x size
		200,             // initial y size
		NULL,            // parent window handle
		NULL,            // window menu handle
		hInstance,       // program instance handle
		NULL) ;          // creation parameters
	 
	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;
	
	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}

	return 0;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc ;
	PAINTSTRUCT ps ;
	RECT        rect ;
	
	switch (message)
	{{
	
	case WM_CREATE:
	{
		return 0 ;
	}
	
	case WM_PAINT:
	{
		hdc = BeginPaint (hwnd, &ps) ;
		
		GetClientRect (hwnd, &rect) ;          
		Ellipse(hdc, 0,0, rect.right, rect.bottom);
		DrawText (hdc, TEXT ("Hello, Windows 98!"), -1, &rect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER) ;

		EndPaint (hwnd, &ps) ;
		return 0 ;
	}  
	case WM_DESTROY:
	{
		PostQuitMessage (0) ;
		return 0 ;
	}
	
	}}
	
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
