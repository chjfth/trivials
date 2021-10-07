#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Psapi.h>
#include <Userenv.h>
#include <locale.h>
#include <assert.h>
#include <tchar.h>

int sret = 0; // success ret
int eret = 0; // error ret
DWORD winerr = 0;

TCHAR soutput[MAX_PATH];
TCHAR eoutput[MAX_PATH];

#define SMALL_BUFCHARS 5

#ifdef UNICODE
#define Memset wmemset
#else
#define Memset memset
#endif

#define RESET_OUTPUT do { \
	SetLastError(0); \
	Memset(soutput, _T('*'), MAX_PATH); \
	Memset(eoutput, _T('*'), MAX_PATH); \
} while(0)


void see_GetKeyNameText()
{
	RESET_OUTPUT;
	LPARAM key = 0x000e0001; // Backspace
	sret = GetKeyNameText(key, soutput, MAX_PATH);
	eret = GetKeyNameText(key, eoutput, SMALL_BUFCHARS);
	winerr = GetLastError();
}

void see_GetClassName()
{
	RESET_OUTPUT;
	HWND hwnd = GetDesktopWindow();
	sret = GetClassName(hwnd, soutput, MAX_PATH); // BufCHARS-1
	// -- #32769
	eret = GetClassName(hwnd, eoutput, SMALL_BUFCHARS); // T
	winerr = GetLastError();
}

void see_GetTextFace()
{
	RESET_OUTPUT;
	HDC hdc = GetDC(NULL);
	sret = GetTextFace(hdc, MAX_PATH, soutput); // BufCHARS
	// -- System
	eret = GetTextFace(hdc, SMALL_BUFCHARS, eoutput); // T+1
	winerr = GetLastError();

	ReleaseDC(NULL, hdc);
}

void see_GetModuleFileName()
{
	RESET_OUTPUT;
	sret = GetModuleFileName(NULL, soutput, MAX_PATH);
	// -- D:\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetModuleFileName(NULL, eoutput, SMALL_BUFCHARS);
	winerr = GetLastError();
}

void see_GetModuleFileName_0buf()
{
	RESET_OUTPUT;
	sret = GetModuleFileName(NULL, soutput, MAX_PATH);
	// -- D:\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetModuleFileName(NULL, NULL, 0);
	winerr = GetLastError();
}


void see_GetProcessImageFileName()
{
	RESET_OUTPUT;
	sret = GetProcessImageFileName(GetCurrentProcess(), soutput, MAX_PATH);
	// -- \Device\HarddiskVolume3\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetProcessImageFileName(GetCurrentProcess(), eoutput, SMALL_BUFCHARS);
	winerr = GetLastError();
}

void see_GetSystemDirectory()
{
	RESET_OUTPUT;
	sret = GetSystemDirectory(soutput, MAX_PATH);
	// -- C:\Windows\system32
	eret = GetSystemDirectory(eoutput, SMALL_BUFCHARS); // T+1
	winerr = GetLastError();
}

void see_GetCurrentDirectory()
{
	RESET_OUTPUT;
	sret = GetCurrentDirectory(MAX_PATH, soutput);
	// -- D:\gitw\trivials\mswin\seeRetBuf
	eret = GetCurrentDirectory(SMALL_BUFCHARS, eoutput); // T+1
	winerr = GetLastError();
}

void see_GetTempPath()
{
	RESET_OUTPUT;
	sret = GetTempPath(MAX_PATH, soutput);
	// -- C:\Users\win7evn\AppData\Local\Temp\ 
	eret = GetTempPath(SMALL_BUFCHARS, eoutput); // T+1
	winerr = GetLastError();
}

void see_GetEnvironmentVariable()
{
	RESET_OUTPUT;
	TCHAR *envvar = _T("HOMEPATH");
	sret = GetEnvironmentVariable(envvar, soutput, MAX_PATH);
	// -- \Users\win7evn
	eret = GetEnvironmentVariable(envvar, eoutput, SMALL_BUFCHARS); // T+1
	winerr = GetLastError();
}

void see_GetDefaultUserProfileDirectory()
{
	RESET_OUTPUT;
	sret = MAX_PATH;
	BOOL succ1 = GetDefaultUserProfileDirectory(soutput, (LPDWORD)&sret);
	// -- C:\Users\Default
	eret = SMALL_BUFCHARS;
	BOOL succ2 = GetDefaultUserProfileDirectory(eoutput, (LPDWORD)&eret); // T+1
		// Windows bug: partial filling but missing NUL.
	winerr = GetLastError();
}

void see_GetLocaleInfo()
{
	RESET_OUTPUT;
	LCID lcid = LOCALE_USER_DEFAULT;
	LCTYPE lctype = LOCALE_SLONGDATE;
	sret = GetLocaleInfo(lcid, lctype, soutput, MAX_PATH);
	// -- MMMM d, yyyy
	eret = GetLocaleInfo(lcid, lctype, eoutput, SMALL_BUFCHARS); // 0
	winerr = GetLastError();
}

void see_GetLocaleInfo_0buf()
{
	RESET_OUTPUT;
	LCID lcid = LOCALE_USER_DEFAULT;
	LCTYPE lctype = LOCALE_SLONGDATE;
	sret = GetLocaleInfo(lcid, lctype, soutput, MAX_PATH);
	// -- MMMM d, yyyy
	_tprintf(_T("LOCALE_SLONGDATE: %s\n"), soutput);
	eret = GetLocaleInfo(lcid, lctype, eoutput, 0); // ReqCHARS+1
	winerr = GetLastError();
}

void see_FormatMessage()
{
	RESET_OUTPUT;
	DWORD sampleerr = ERROR_FILE_NOT_FOUND;
	sret = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 
		sampleerr, 
		0, // LANGID
		soutput, MAX_PATH,
		NULL);
	// -- The system cannot find the file specified.
	eret = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 
		sampleerr, 
		0, // LANGID
		eoutput, SMALL_BUFCHARS,
		NULL);
	winerr = GetLastError();
}

#include <InitGuid.h>
DEFINE_GUID(myguid,
	0x20161110, 0x5434, 0x11d3, 0xb8, 0x90, 0x0, 0xc0, 0x4f, 0xad, 0x51, 0x71);

void see_StringFromGUID2()
{
	RESET_OUTPUT;
	sret = StringFromGUID2(myguid, soutput, MAX_PATH);
	// -- {20161110-5434-11D3-B890-00C04FAD5171}
	eret = StringFromGUID2(myguid, eoutput, SMALL_BUFCHARS);
	winerr = GetLastError();
}


int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, ""); // only for printf Chinese chars purpose

	see_GetKeyNameText();
	see_GetClassName();

	see_GetTextFace();
	
	see_GetModuleFileName();
	see_GetModuleFileName_0buf();
	
	see_GetProcessImageFileName();
	
	see_GetSystemDirectory();
	see_GetCurrentDirectory();

	see_GetTempPath();
	see_GetEnvironmentVariable();

	see_GetDefaultUserProfileDirectory();

	see_GetLocaleInfo();
	see_GetLocaleInfo_0buf();

	see_FormatMessage();

	see_StringFromGUID2();

	return 0;
}

