#include <cwchar>
#include <stdarg.h>
#include <assert.h>
#include <strsafe.h>
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#define WINDOWCLASSNAME L"WinRectByDpi"
#define REFRESH_BTN_TEXT L"Refresh (Ctrl+ to see true boundary)"
#define NOTEPAD_MISSING_TEXT L"Please open a Notepad window first, then click [Refresh]."

const int WinWidth = 720;
const int WinHeight = 400;

const int BtnHeight = 30;
enum {
	CMD_REFRESH = 1,
	CMD_PAINT_SLATE,
};

HINSTANCE g_hInst = NULL;

static WCHAR g_szrcinfo[100];
static WCHAR g_szedtprefix[80];
static WCHAR g_szedit[800];
const int SIZEEDIT = ARRAYSIZE(g_szedit);

DPI_AWARENESS_CONTEXT g_thiswnd_dpictx = nullptr; // init to an invalid value
UINT g_thiswnd_dpi = 0;
HWND g_targethwnd = nullptr;

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

const WCHAR *DPIContextStr(DPI_AWARENESS_CONTEXT dpictx)
{
	if (AreDpiAwarenessContextsEqual(dpictx, DPI_AWARENESS_CONTEXT_UNAWARE))
		return L"DPI_AWARENESS_CONTEXT_UNAWARE";
	else if (AreDpiAwarenessContextsEqual(dpictx, DPI_AWARENESS_CONTEXT_SYSTEM_AWARE))
		return L"DPI_AWARENESS_CONTEXT_SYSTEM_AWARE";
	else if (AreDpiAwarenessContextsEqual(dpictx, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE))
		return L"DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE";
	else if (AreDpiAwarenessContextsEqual(dpictx, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
		return L"DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2";
	else if (AreDpiAwarenessContextsEqual(dpictx, DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED))
		return L"DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED";
	else
		return nullptr; // means invalid input
}

void GrabNotepadHwndInfo(HWND hwnd, DPI_AWARENESS_CONTEXT dpictx)
{
	// Output info appended to g_szedit[] .
	
	DPI_AWARENESS_CONTEXT oldctx = nullptr;

	if(dpictx!=nullptr)
	{
		const WCHAR *ctxname = DPIContextStr(dpictx);
		
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

	SetThreadDpiAwarenessContext(oldctx);
}

const WCHAR *get_result(DWMNCRENDERINGPOLICY render)
{
	DWORD winerr = 0;
	HWND hwnd = nullptr;
	
	if(!g_targethwnd)
	{
		hwnd = FindWindowW(L"Notepad", NULL);
		if(!hwnd)
		{
			StringCchPrintfW(g_szedit, ARRAYSIZE(g_szedit), L"%s%s", 
                g_szedtprefix, NOTEPAD_MISSING_TEXT);
			return g_szedit;
		}
	}
	else
	{
		hwnd = g_targethwnd;
		if( !IsWindow(hwnd) )
		{
			StringCchPrintfW(g_szedit, ARRAYSIZE(g_szedit), L"%sCannot find target HWND 0x%08X",
				g_szedtprefix, (UINT)hwnd);
			return g_szedit;
		}
	}
	
	const WCHAR *ctxstr = DPIContextStr(g_thiswnd_dpictx);
	StringCchPrintfW(g_szedit, ARRAYSIZE(g_szedit),
		L"%sThis HWND-DPI-context: %s\r\n",
		g_szedtprefix,
		ctxstr ? ctxstr : L"(Unknown value)");
	
	GrabNotepadHwndInfo(hwnd, nullptr);

	GrabNotepadHwndInfo(hwnd, DPI_AWARENESS_CONTEXT_UNAWARE);

	GrabNotepadHwndInfo(hwnd, DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

	GrabNotepadHwndInfo(hwnd, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);

	GrabNotepadHwndInfo(hwnd, DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

	if(render==DWMNCRP_DISABLED || render== DWMNCRP_ENABLED)
	{
		// Disable/Enable non-client area rendering on the window.
		// So that we can see Window-Rect's real boundary.
		HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &render, sizeof(render));
		if (hr != S_OK)
		{
			MessageBoxW(NULL, L"DwmSetWindowAttribute(DWMWA_NCRENDERING_POLICY) fails!", L"Error", MB_OK);
		}

		// test code >>>
		DWMNCRENDERINGPOLICY opolicy = DWMNCRP_USEWINDOWSTYLE;
		HRESULT hrt = DwmGetWindowAttribute(hwnd, DWMWA_NCRENDERING_POLICY, &opolicy, sizeof(opolicy));
		// this always fail with E_INVALIDARG(0X80070057), probably DWMWA_NCRENDERING_POLICY can only be Set, but not Get.

		BOOL enabled = 0;
		hrt = DwmGetWindowAttribute(hwnd, DWMWA_NCRENDERING_ENABLED, &enabled, sizeof(BOOL));
		// -- this is ok
		// test code <<<
	}
	
    return g_szedit;
}

void MyRefreshLayout(HWND htop, HWND hedit, HWND hbutton, HWND hstatic)
{
	RECT rc = {};
	GetClientRect(htop, &rc);

	// upper part
	MoveWindow(hedit, 0, 0, rc.right, rc.bottom - BtnHeight, TRUE);

	// lower left
	MoveWindow(hbutton, 0, rc.bottom - BtnHeight, rc.right - BtnHeight, BtnHeight, TRUE);

	// lower right
	MoveWindow(hstatic, rc.right-BtnHeight, rc.bottom-BtnHeight, BtnHeight, BtnHeight, TRUE);
}

void EraseMySlate(HWND hstatic)
{
	HDC hdc = GetDC(hstatic);

	RECT rcpaint = {};
	GetClientRect(hstatic, &rcpaint); // We do this after set Thread-DPI context.

	FillRect(hdc, &rcpaint, GetStockBrush(WHITE_BRUSH));
	
	ReleaseDC(hstatic, hdc);
}

void PaintMySlate(HWND hstatic, bool keyCtrl, bool keyShift)
{
	EraseMySlate(hstatic);

	DPI_AWARENESS_CONTEXT thread_ctx_old = nullptr;
	DPI_AWARENESS_CONTEXT thread_ctx = DPI_AWARENESS_CONTEXT_UNAWARE;
	const WCHAR *painting_ctx_str = L"DPI_AWARENESS_CONTEXT_UNAWARE";

	if(keyCtrl)
	{
		thread_ctx = DPI_AWARENESS_CONTEXT_SYSTEM_AWARE;
		painting_ctx_str = L"DPI_AWARENESS_CONTEXT_SYSTEM_AWARE";
	}
	else if(keyShift)
	{
		thread_ctx = DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE;
		painting_ctx_str = L"DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE";
	}

	// Temporarily change Thread-DPI context before painting the slate.
	thread_ctx_old = SetThreadDpiAwarenessContext(thread_ctx);

	UINT dpi_childwnd = GetDpiForWindow(hstatic);

	RECT rcpaint = {};
	GetClientRect(hstatic, &rcpaint); // We do this after set Thread-DPI context.

	HDC hdc = GetDC(hstatic);

	// Draw interleaving 1-vpx white / 1-vpx black stripes, on the slate.
	// User can observe whether this stripes are clear or blurry.

	for(int x=0; x<rcpaint.right; x+=2)
	{
		MoveToEx(hdc, x, 0, nullptr);
		LineTo(hdc, x, rcpaint.bottom);
	}

	WCHAR info[800];
	StringCchPrintfW(info, ARRAYSIZE(info), 
		L"This calling thread has temporarily changed its Thread-DPI-context to %s.\r\n\r\n"
		L"During this change, on the bottom right corner slate, I have painted black stripes every other pixel.\r\n\r\n"
		L"The painted width is %d vpx.\r\n\r\n"
		L"If I predict it correctly...\r\n\r\n"
		,
		painting_ctx_str,
		rcpaint.right - rcpaint.left
		);

	DPI_AWARENESS_CONTEXT hwnd_ctx = GetWindowDpiAwarenessContext(hstatic);
	DPI_AWARENESS hwnd_daw = GetAwarenessFromDpiAwarenessContext(hwnd_ctx);
	if (hwnd_daw == DPI_AWARENESS_PER_MONITOR_AWARE)
	{
		StringCchPrintfW(info, ARRAYSIZE(info),
			L"%sThe stripes will be crispy (not blurry) on every monitor.",
		info);
	}
	else
	{
		assert(g_thiswnd_dpi != 0);
		StringCchPrintfW(info, ARRAYSIZE(info),
			L"%sThe stripes will be blurry(bitmap-stretched, not crispy) unless they are displayed on the %u-DPI monitor "
			L"(or say, the monitor with scale factor %d%%) .\r\n\r\n"
			L"Hint: To paint with other Thread-DPI-context, click with Ctrl or Shift."
			,
			info,
			g_thiswnd_dpi,
			MulDiv(g_thiswnd_dpi, 100, 96) );
	}
	
	MessageBox(hstatic, info, L"Info", MB_OK);
	
	ReleaseDC(hstatic, hdc);

	SetThreadDpiAwarenessContext(thread_ctx_old);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hedit = NULL, hbutton = NULL, hstatic = NULL;
	
	switch (message)
	{{
	case WM_CREATE:
	{
		// Create text display area.
		hedit = CreateWindowExW(WS_EX_LEFT, L"Edit",
			NOTEPAD_MISSING_TEXT,
			WS_CHILD | WS_VISIBLE | ES_LEFT | ES_READONLY | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL,
			0, 0, 0, 0,
			hWnd, nullptr, g_hInst, nullptr);

		// Create [Refresh] button.
		hbutton = CreateWindowExW(WS_EX_LEFT, L"Button",
			REFRESH_BTN_TEXT,
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			0, 0, 0, 0,
			hWnd, 
			(HMENU)CMD_REFRESH, 
			g_hInst, nullptr);

		// Create bottom-right corner static slate
		hstatic = CreateWindowExW(0, L"Static",
			L"ABC", // empty text
			WS_CHILD | WS_VISIBLE | SS_NOTIFY | SS_WHITEFRAME, // | SS_WHITERECT,
			0, 0, 0, 0,
			hWnd, 
			(HMENU)CMD_PAINT_SLATE,
			g_hInst, nullptr
		);

		MyRefreshLayout(hWnd, hedit, hbutton, hstatic);
			
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
		MyRefreshLayout(hWnd, hedit, hbutton, hstatic);
		return 0;
	}
		
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	case WM_COMMAND:
	{
		bool keyShift = GetKeyState(VK_SHIFT) < 0; // "<0" means pressed
		bool keyCtrl = GetKeyState(VK_CONTROL) < 0;
		DWMNCRENDERINGPOLICY render = DWMNCRP_USEWINDOWSTYLE; // 0
		if(keyCtrl)
		{
			render = keyShift ? DWMNCRP_ENABLED : DWMNCRP_DISABLED;
		}
				
		int wmId = LOWORD(wParam);
			
		if(wmId==CMD_REFRESH)
		{
			SetWindowTextW(hedit, get_result(render));
			return 0;
		}
		else if(wmId==CMD_PAINT_SLATE)
		{
			PaintMySlate(hstatic, keyCtrl, keyShift);
			return 0;
		}
		break;
	}

	}}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR cmdparams, _In_ int nCmdShow)
{
	// cmdparams can contain one or two params:
	// (1) With a leading minus char, telling initial Thread-DPI-context to use: -1, -2, -3, -4, or -5 .
	// (2) Not with a leading minus char, it is the hex-form window handle(inputHwnd), so we will
	//     investigate inputHwnd instead of a Notepad window.
	//  Example:
	//  WinRectByDpi -2
	//  WinRectByDpi 40b42
	//  WinRectByDpi -3 40b42
	//  WinRectByDpi -3 0x40b42

	int numargs = 0;
	LPWSTR *args = CommandLineToArgvW(GetCommandLineW(), &numargs);
	WCHAR szTitle[100] = WINDOWCLASSNAME;

	if (numargs>1)
	{
		StringCchPrintfW(g_szedtprefix, ARRAYSIZE(g_szedtprefix),
			L"Run with parameter: %s\r\n", cmdparams
			); // cmdparams does not contain args[0]

		LPWSTR *args_adv = args+1;

		if(args_adv[0][0]==L'-')
		{
			DPI_AWARENESS_CONTEXT dpictx = (DPI_AWARENESS_CONTEXT)_wtoi(args_adv[0]);
			const WCHAR *dpictx_str = DPIContextStr(dpictx);

			SetThreadDpiAwarenessContext(dpictx);

			args_adv++;
		}
		
		if(args_adv-args < numargs)
		{
			g_targethwnd = (HWND) wcstoul(args_adv[0], NULL, 16);

			StringCchPrintfW(szTitle, ARRAYSIZE(szTitle), L"%s (#%08X)", 
				WINDOWCLASSNAME, (UINT)g_targethwnd);
		}
	}

	// Query the DPI-context value, this tells us the actual result.
	// And we will display it in our main UI later.
	// Chj: The significance of this Thread-DPI-context is that it determines the HWND-DPI-context
	// it(this thread) later creates. So I save it an a global var with "thiswnd" in its name.
	g_thiswnd_dpictx = GetThreadDpiAwarenessContext();
	if (GetAwarenessFromDpiAwarenessContext(g_thiswnd_dpictx) != DPI_AWARENESS_PER_MONITOR_AWARE)
	{
		// -- Chj: Yes, g_thiswnd_dpi is meaningful only when non Per-monitor-aware.
		g_thiswnd_dpi = GetDpiForSystem();
	}
	
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
		szTitle,
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
