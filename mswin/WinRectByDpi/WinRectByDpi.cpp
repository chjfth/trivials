#include <cwchar>
#include <stdarg.h>
#include <strsafe.h>
#include <windows.h>
#include <windowsx.h>

#define WINDOWCLASSNAME            L"WinRectByDpi"

const int WinWidth = 600;
const int WinHeight = 400;

const int BtnHeight = 30;
const int CMD_REFRESH = 1;

HINSTANCE g_hInst = NULL;

static WCHAR g_szrcinfo[100];
static WCHAR g_szedit[800];
const int SIZEEDIT = ARRAYSIZE(g_szedit);

void DbgPrint(const WCHAR *fmt, ...)
{
	WCHAR info[200] = {};
	va_list args;
	va_start(args, fmt);

	vswprintf(info, ARRAYSIZE(info), fmt, args);
	OutputDebugStringW(info);
	
	va_end(args);
}

const WCHAR *str_rcinfo(const RECT &rc)
{
	StringCchPrintfW(g_szrcinfo, ARRAYSIZE(g_szrcinfo),
		L"LT(%d, %d) RB(%d, %d) [%dx%d]",
		rc.left, rc.top, rc.right, rc.bottom, (rc.right - rc.left), (rc.bottom - rc.top)
	);
	return g_szrcinfo;
}

void DoTest(HWND hwnd, DPI_AWARENESS_CONTEXT dpictx, const WCHAR *ctxname)
{
	// Output info appended to g_szedit[] .
	
	DPI_AWARENESS_CONTEXT oldctx = nullptr;

	if(dpictx!=nullptr)
	{
		StringCchPrintfW(g_szedit, SIZEEDIT, L"%sSet %s\r\n", g_szedit,
			ctxname);
		
		oldctx = SetThreadDpiAwarenessContext(dpictx);
		if(oldctx==nullptr)
		{
			StringCchPrintfW(g_szedit, SIZEEDIT, L"%sSetThreadDpiAwarenessContext(%s) error!\r\n", g_szedit,
				ctxname);
			return;
		}
	}
	
	RECT rc = {};
	GetWindowRect(hwnd, &rc);

	StringCchPrintfW(g_szedit, SIZEEDIT, L"%sNotepad GetWindowRect: %s\r\n\r\n", g_szedit,
		str_rcinfo(rc));
}

const WCHAR *get_result()
{
	DWORD winerr = 0;
	HWND hwnd = FindWindow(L"Notepad", NULL);
	if(!hwnd)
	{
		return L"Cannot find a Notepad window.";
	}

	g_szedit[0] = 0;
	
	DoTest(hwnd, nullptr, L"");

	DoTest(hwnd, DPI_AWARENESS_CONTEXT_UNAWARE, L"DPI_AWARENESS_CONTEXT_UNAWARE");

	DoTest(hwnd, DPI_AWARENESS_CONTEXT_SYSTEM_AWARE, L"DPI_AWARENESS_CONTEXT_SYSTEM_AWARE");

	DoTest(hwnd, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE, L"DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE");

	DoTest(hwnd, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2, L"DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2");

    return g_szedit;
}

void MyRefreshLayout(HWND htop, HWND hedit, HWND hbutton)
{
	RECT rc = {};
	GetClientRect(htop, &rc);

	MoveWindow(hedit, 0, 0, rc.right, rc.bottom - BtnHeight, TRUE);

	MoveWindow(hbutton, 0, rc.bottom - BtnHeight, rc.right, BtnHeight, TRUE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hedit = NULL, hbutton = NULL;
	
	switch (message)
	{{
	case WM_CREATE:
	{
		// Create text display area.
		hedit = CreateWindowExW(WS_EX_LEFT, L"Edit",
			L"Please open a Notepad window first, then click [Refresh].",
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL,
			0, 0, 0, 0,
			hWnd, nullptr, g_hInst, nullptr);

		// Create [Refresh] button.
		hbutton = CreateWindowExW(WS_EX_LEFT, L"Button",
			L"Refresh",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0,
			hWnd, 
			(HMENU)CMD_REFRESH, 
			g_hInst, nullptr);

		MyRefreshLayout(hWnd, hedit, hbutton);
			
		::PostMessageW(hWnd, WM_COMMAND, CMD_REFRESH, 0);
		break;
	}

	case WM_CTLCOLORSTATIC:
	{
		return (INT_PTR)GetStockBrush(WHITE_BRUSH); // Set editbox bgcolor to white
	}

	// On DPI change resize the window, scale the font, and update
	// the DPI-info string
	case WM_DPICHANGED:
	{
		UINT uDpi = HIWORD(wParam);
		RECT *prcNewScale = reinterpret_cast<RECT*>(lParam);
		DbgPrint(L"[WM_DPICHANGED] New DPI=%d\r\n", uDpi);
		break;
	}

	case WM_SIZE:
	{
		MyRefreshLayout(hWnd, hedit, hbutton);
		return 0;
	}
		
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		if(wmId==CMD_REFRESH)
		{
			SetWindowTextW(hedit, get_result());
			return 0;
		}
		break;
	}

	}}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
	WNDCLASSEXW wcex = {};

	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = WINDOWCLASSNAME;

	RegisterClassExW(&wcex);

	g_hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowExW(
		0,
		WINDOWCLASSNAME, 
		WINDOWCLASSNAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, WinWidth, WinHeight, NULL, NULL,
		g_hInst, NULL);
	
	ShowWindow(hWnd, SW_SHOWNORMAL);

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}
