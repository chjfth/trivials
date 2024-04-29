#define WIN32_LEAN_AND_MEAN
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
	static TCHAR szAppName[] = TEXT ("ChgWinstyle") ;
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
	 
	hwnd = CreateWindow (szAppName, // window class name
		TEXT ("ChgWinstyle - Change window-style live"), // window caption
		WS_OVERLAPPEDWINDOW,        // window style
		222,              // initial x position
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

bool g_click_top = true;

void click_which(HWND hwnd, int x, int y)
{
	RECT rect = {};
	GetClientRect (hwnd, &rect);

	if(y < rect.bottom/2)
		g_click_top = true;
	else
		g_click_top = false;
}


void redraw_my_window(HWND hwnd)
{
	if(g_click_top)
	{
		SetWindowPos(hwnd, 0, 0,0,0,0, 
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}
	else
	{
		UINT flags = RDW_FRAME | RDW_ERASE | RDW_INVALIDATE | RDW_ALLCHILDREN 
			| RDW_ERASENOW | RDW_UPDATENOW ;
		RedrawWindow(hwnd, NULL, NULL, flags);
	}

	UpdateWindow(hwnd);
}

void check_style_effective(HWND hwnd, bool isExStyle, ULONG user_bits)
{
	ULONG effective = (ULONG)GetWindowLongPtr(hwnd, isExStyle?GWL_EXSTYLE:GWL_STYLE);
	if(user_bits != effective)
	{
		vaDbg(_T("%s mismatch! user sets: 0x%08lX , effective: 0x%08lX"),
			isExStyle ? "WS_EX_xxx" : "WS_xxx", 
			user_bits, 
			effective);
	}
}

void ChgExStyle(HWND hwnd, ULONG add_bits, ULONG del_bits)
{
	ULONG exStyle = (ULONG)GetWindowLongPtr(hwnd, GWL_EXSTYLE);

	exStyle |= add_bits;
	exStyle &= ~del_bits;

	SetWindowLongPtr(hwnd, GWL_EXSTYLE, exStyle);
	check_style_effective(hwnd, true, exStyle);

	redraw_my_window(hwnd);
}

void SetExStyle(HWND hwnd, ULONG user_bits)
{
	SetWindowLongPtr(hwnd, GWL_EXSTYLE, user_bits);
	check_style_effective(hwnd, true, user_bits);

	redraw_my_window(hwnd);
}

void ChgStyle(HWND hwnd, ULONG add_bits, ULONG del_bits)
{
	ULONG Style = (ULONG)GetWindowLongPtr(hwnd, GWL_STYLE);

	Style |= add_bits;
	Style &= ~del_bits;

	ULONG res = (ULONG)SetWindowLongPtr(hwnd, GWL_STYLE, Style);
	check_style_effective(hwnd, false, Style);

	redraw_my_window(hwnd);
}

void SetStyle(HWND hwnd, ULONG user_bits)
{
	SetWindowLongPtr(hwnd, GWL_STYLE, user_bits);
	check_style_effective(hwnd, false, user_bits);

	redraw_my_window(hwnd);
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
		hdc = BeginPaint (hwnd, &ps);
		
		GetClientRect (hwnd, &rect);

		RECT uprect = {rect.left, rect.top, rect.right, rect.bottom/2};
		RECT downrect = {rect.left, rect.bottom/2, rect.right, rect.bottom};

		FrameRect(hdc, &uprect, GetStockBrush(BLACK_BRUSH));
		DrawText(hdc, TEXT("Click here to call SetWindowPos( )."), -1, &uprect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		FrameRect(hdc, &downrect, GetStockBrush(BLACK_BRUSH));
		DrawText(hdc, TEXT("Click here to call RedrawWindow( )."), -1, &downrect,
			DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		EndPaint (hwnd, &ps) ;
		return 0 ;
	}  

	case WM_LBUTTONDOWN:
	{
		click_which(hwnd, LOWORD(lParam), HIWORD(lParam));
		
		ChgExStyle(hwnd, WS_EX_TOOLWINDOW, 0);
		
		return 0;
	}
	case WM_RBUTTONDOWN:
	{
		click_which(hwnd, LOWORD(lParam), HIWORD(lParam));
		
		ChgExStyle(hwnd, 0, WS_EX_TOOLWINDOW);
		
		return 0;
	}
	
	case WM_DESTROY:
	{
		PostQuitMessage (0) ;
		return 0 ;
	}
	
	}}
	
	return DefWindowProc (hwnd, message, wParam, lParam) ;
}
