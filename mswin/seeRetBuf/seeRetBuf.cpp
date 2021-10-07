#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <Psapi.h>
#include <Userenv.h>
#include <assert.h>
#include <tchar.h>

int sret = 0; // success ret
int eret = 0; // error ret
DWORD winerr = 0;

TCHAR soutput[MAX_PATH];
TCHAR eoutput[MAX_PATH];

#define SMALL_BUFCHARS 6

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

void see_GetModuleFileName()
{
	RESET_OUTPUT;
	sret = GetModuleFileName(NULL, soutput, MAX_PATH);
	// -- D:\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetModuleFileName(NULL, eoutput, SMALL_BUFCHARS);
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

void see_GetLocaleInfo_smallbuf()
{
	RESET_OUTPUT;
	LCID lcid = LOCALE_USER_DEFAULT;
	LCTYPE lctype = LOCALE_SLONGDATE;
	sret = GetLocaleInfo(lcid, lctype, soutput, MAX_PATH);
	// -- 
	eret = GetLocaleInfo(lcid, lctype, eoutput, SMALL_BUFCHARS); // 0
	winerr = GetLastError();
}

void see_GetLocaleInfo_0buf()
{
	RESET_OUTPUT;
	LCID lcid = LOCALE_USER_DEFAULT;
	LCTYPE lctype = LOCALE_SLONGDATE;
	sret = GetLocaleInfo(lcid, lctype, soutput, MAX_PATH);
	// -- 
	eret = GetLocaleInfo(lcid, lctype, eoutput, 0); // ReqCHARS+1
	winerr = GetLastError();
}


int _tmain(int argc, _TCHAR* argv[])
{
	see_GetKeyNameText();
	
	see_GetModuleFileName();
	
	see_GetProcessImageFileName();
	
	see_GetSystemDirectory();
	see_GetCurrentDirectory();

	see_GetTempPath();
	see_GetEnvironmentVariable();

	see_GetDefaultUserProfileDirectory();

	see_GetLocaleInfo_smallbuf();
	see_GetLocaleInfo_0buf();


	return 0;
}

