// A program to see various Windows API returned buffer-size behavior,
// especially when user buffer is too small.
// This program prints a CSV file summarizing each WinAPI's traits based on live system run.
//
// Author: Jimm Chen, 2021.10
//
// Complie the program with:
//
//    cl /DUNICODE /D_UNICODE seeRetBuf.cpp /link user32.lib psapi.lib userenv.lib gdi32.lib ole32.lib

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
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

#define SMALL_Usersize 5

#ifdef UNICODE
#define Memset wmemset
#else
#define Memset memset
#endif

#define NULCHAR _T('\0')
#define BADCHAR _T('*')

#define RESET_OUTPUT do { \
	SetLastError(0); \
	Memset(soutput, BADCHAR, MAX_PATH); \
	Memset(eoutput, BADCHAR, MAX_PATH); \
} while(0)


enum BufSmallRet_et // Ret trait when buffer small
{
	BSR_Zero = 0,
	BSR_Usersize,
	BSR_UsersizeLess1,
	BSR_Total, // C99 snprintf behavior, not seen on winapi
	BSR_TotalPlus1,

	BSR_BUG
};

enum BufSmallFill_et // User buffer filling trait when buffer small
{
	BSF_Pure_NUL = 0,
	BSF_LeaveAsIs,
	BSF_PartialAndNUL,
	BSF_PartialNoNUL,

	BSF_BUG
};

enum BufEnoughRet_et // Ret trait when buffer enough
{
	ReturnT = 0,      // Return exactly "total" size, the correct result's strlen().
	                  // "Total"(T) does not count trailing NUL 
	ReturnTplus1 = 1, // Return T+1

	ReturnBug
};

struct ApiTrait_et
{
	BufSmallRet_et bs_ret;
	DWORD bs_winerr;
	BufSmallFill_et bs_fill;
	BufEnoughRet_et good_ret;
};

BufSmallRet_et BufSmallRet_conclude(int user_size, int eret_size, const TCHAR *soutput)
{
	int total_size = _tcslen(soutput);

	if(eret_size==total_size)
		return BSR_Total;
	
	if(eret_size==total_size+1)
		return BSR_TotalPlus1;

	if(eret_size==user_size)
		return BSR_Usersize;

	if(user_size>1 && eret_size==user_size-1)
		return BSR_UsersizeLess1;

	if(eret_size==BSR_Zero)
		return BSR_Zero;

	return BSR_BUG;
}

BufSmallFill_et BufSmallFill_conclude(
	int user_size, // user input buffer size
	int eret_size, // returned buffer size on small buffer(e: error)
	const TCHAR *ebuf, const TCHAR *sbuf)
{
	if(ebuf[0]==NULCHAR)
		return BSF_Pure_NUL;

	if(ebuf[0]==BADCHAR && ebuf[1]==BADCHAR)
		return BSF_LeaveAsIs;

	if(_tcsncmp(ebuf, sbuf, user_size-1)==0)
	{
		if( ebuf[user_size-1]==NULCHAR )
			return BSF_PartialAndNUL;
		else if ( ebuf[user_size-1]==sbuf[user_size-1] )
		{
			return BSF_PartialNoNUL;
		}
		else if ( ebuf[user_size-1]==BADCHAR )
		{
			// This is special wacky behavior of GetDefaultUserProfileDirectory().
			// Microsoft programmer forgot to fill the final char.
			return BSF_PartialNoNUL;
		}
		else
			return BSF_BUG;
	}
	
	return BSF_BUG;
}

BufEnoughRet_et BufEnoughRet_conclude(
	int sret_size, // returned buffer size on success
	const TCHAR *soutput // output buffer of success case
	)
{
	int total_size = _tcslen(soutput);

	if(sret_size==total_size)
		return ReturnT;

	if(sret_size==total_size+1)
		return ReturnTplus1;

	return ReturnBug;
}

void Prn(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	_vtprintf(fmt, args);

	va_end(args);
}

void ReportTraits(const TCHAR *apiname,
	int user_size, int eret_size, DWORD winerr,
	const TCHAR *eoutput, 
	int sret_size, const TCHAR *soutput)

{
	Prn(_T("%s,"), apiname);

	ApiTrait_et t = {};
	
	t.bs_ret = BufSmallRet_conclude(user_size, eret_size, soutput);
	switch(t.bs_ret)
	{
	case BSR_Zero: 
		Prn(_T("Zero")); break;
	case BSR_Usersize: 
		Prn(_T("Usersize")); break;
	case BSR_UsersizeLess1:
		Prn(_T("Usersize-1")); break;
	case BSR_Total:
		Prn(_T("Total")); break;
	case BSR_TotalPlus1:
		Prn(_T("Total+1")); break;
	default:
		Prn(_T("[BUG]"));
	}
	
	Prn(_T(","));

	t.bs_winerr = winerr;
	Prn(_T("%d"), t.bs_winerr);

	Prn(_T(","));

	t.bs_fill = BufSmallFill_conclude(user_size, eret_size, eoutput, soutput);
	switch(t.bs_fill)
	{
	case BSF_Pure_NUL:
		Prn(_T("No+NUL")); break;
	case BSF_LeaveAsIs:
		Prn(_T("No*")); break;
	case BSF_PartialAndNUL:
		Prn(_T("Yes+NUL")); break;
	case BSF_PartialNoNUL:
		Prn(_T("Yes*")); break;
	default:
		Prn(_T("[BUG]"));
	}
	
	Prn(_T(","));

	t.good_ret = BufEnoughRet_conclude(sret_size, soutput);
	switch(t.good_ret)
	{
	case ReturnT:
		Prn(_T("T")); break;
	case ReturnTplus1:
		Prn(_T("T+1")); break;
	default:
		Prn(_T("[BUG]"));
	}

	Prn(_T("\n"));
}

//////////////////////////////////////////////////////////////////////////

#define REPORT_API_TRAITS_s(apiname_s) do { \
	ReportTraits( apiname_s, SMALL_Usersize, eret, winerr, eoutput, sret, soutput); \
} while(0)

#define REPORT_API_TRAITS(apiname) REPORT_API_TRAITS_s(_T(#apiname))

void see_GetKeyNameText()
{
	RESET_OUTPUT;
	LPARAM key = 0x000e0001; // Backspace
	sret = GetKeyNameText(key, soutput, MAX_PATH);
	eret = GetKeyNameText(key, eoutput, SMALL_Usersize);
	winerr = GetLastError();
	REPORT_API_TRAITS(GetKeyNameText);
}

void see_GetClassName()
{
	RESET_OUTPUT;
	HWND hwnd = GetDesktopWindow();
	sret = GetClassName(hwnd, soutput, MAX_PATH); // Usersize-1
	// -- #32769
	eret = GetClassName(hwnd, eoutput, SMALL_Usersize); // T
	winerr = GetLastError();
	REPORT_API_TRAITS(GetClassName);
}

void see_GetTextFace()
{
	RESET_OUTPUT;
	HDC hdc = GetDC(NULL);
	
	sret = GetTextFace(hdc, MAX_PATH, soutput); // Usersize
	// -- System
	eret = GetTextFace(hdc, SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();

	REPORT_API_TRAITS(GetTextFace);

	ReleaseDC(NULL, hdc);
}

void see_GetModuleFileName()
{
	RESET_OUTPUT;
	sret = GetModuleFileName(NULL, soutput, MAX_PATH);
	// -- D:\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetModuleFileName(NULL, eoutput, SMALL_Usersize);
	winerr = GetLastError();
	REPORT_API_TRAITS(GetModuleFileName);
}

void see_GetModuleFileName_0buf()
{
	RESET_OUTPUT;
	sret = GetModuleFileName(NULL, soutput, MAX_PATH);
	// -- D:\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetModuleFileName(NULL, NULL, 0);
	winerr = GetLastError();
	REPORT_API_TRAITS_s(_T("GetModuleFileName(0buf)"));
}


void see_GetProcessImageFileName()
{
	RESET_OUTPUT;
	sret = GetProcessImageFileName(GetCurrentProcess(), soutput, MAX_PATH);
	// -- \Device\HarddiskVolume3\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetProcessImageFileName(GetCurrentProcess(), eoutput, SMALL_Usersize);
	winerr = GetLastError();
	REPORT_API_TRAITS(GetProcessImageFileName);
}

void see_GetSystemDirectory()
{
	RESET_OUTPUT;
	sret = GetSystemDirectory(soutput, MAX_PATH);
	// -- C:\Windows\system32
	eret = GetSystemDirectory(eoutput, SMALL_Usersize); // T+1
	winerr = GetLastError();
	REPORT_API_TRAITS(GetSystemDirectory);
}

void see_GetCurrentDirectory()
{
	RESET_OUTPUT;
	sret = GetCurrentDirectory(MAX_PATH, soutput);
	// -- D:\gitw\trivials\mswin\seeRetBuf
	eret = GetCurrentDirectory(SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();
	REPORT_API_TRAITS(GetCurrentDirectory);
}

void see_GetTempPath()
{
	RESET_OUTPUT;
	sret = GetTempPath(MAX_PATH, soutput);
	// -- C:\Users\win7evn\AppData\Local\Temp\ 
	eret = GetTempPath(SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();
	REPORT_API_TRAITS(GetTempPath);
}

void see_GetEnvironmentVariable()
{
	RESET_OUTPUT;
	TCHAR *envvar = _T("HOMEPATH");
	sret = GetEnvironmentVariable(envvar, soutput, MAX_PATH);
	// -- \Users\win7evn
	eret = GetEnvironmentVariable(envvar, eoutput, SMALL_Usersize); // T+1
	winerr = GetLastError();
	REPORT_API_TRAITS(GetEnvironmentVariable);
}

void see_GetDefaultUserProfileDirectory()
{
	RESET_OUTPUT;
	sret = MAX_PATH;
	BOOL succ1 = GetDefaultUserProfileDirectory(soutput, (LPDWORD)&sret);
	// -- C:\Users\Default
	eret = SMALL_Usersize;
	BOOL succ2 = GetDefaultUserProfileDirectory(eoutput, (LPDWORD)&eret); // T+1
		// Windows bug: partial filling but missing NUL.
	winerr = GetLastError();
	REPORT_API_TRAITS(GetDefaultUserProfileDirectory);
}

void see_GetLocaleInfo()
{
	RESET_OUTPUT;
	LCID lcid = LOCALE_USER_DEFAULT;
	LCTYPE lctype = LOCALE_SLONGDATE;
	sret = GetLocaleInfo(lcid, lctype, soutput, MAX_PATH);
	// -- MMMM d, yyyy
	eret = GetLocaleInfo(lcid, lctype, eoutput, SMALL_Usersize); // 0
	winerr = GetLastError();
	REPORT_API_TRAITS(GetLocaleInfo);
}

void see_GetLocaleInfo_0buf()
{
	RESET_OUTPUT;
	LCID lcid = LOCALE_USER_DEFAULT;
	LCTYPE lctype = LOCALE_SLONGDATE;
	sret = GetLocaleInfo(lcid, lctype, soutput, MAX_PATH);
	// -- MMMM d, yyyy
//	_tprintf(_T("LOCALE_SLONGDATE: %s\n"), soutput);
	eret = GetLocaleInfo(lcid, lctype, eoutput, 0); // Usersize+1
	winerr = GetLastError();
	REPORT_API_TRAITS_s(_T("GetLocaleInfo(0buf)"));
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
		eoutput, SMALL_Usersize,
		NULL);
	winerr = GetLastError();
	REPORT_API_TRAITS(FormatMessage);
}

#include <InitGuid.h>
DEFINE_GUID(myguid,
	0x20161110, 0x5434, 0x11d3, 0xb8, 0x90, 0x0, 0xc0, 0x4f, 0xad, 0x51, 0x71);

void see_StringFromGUID2()
{
	// Note: StringFromGUID2() has Unicode version only

	RESET_OUTPUT;
	sret = StringFromGUID2(myguid, soutput, MAX_PATH);
	// -- {20161110-5434-11D3-B890-00C04FAD5171}
	eret = StringFromGUID2(myguid, eoutput, SMALL_Usersize);
	winerr = GetLastError();
	REPORT_API_TRAITS(StringFromGUID2);
}


int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, ""); // only for printf Chinese chars purpose

	Prn(_T("WinAPI,BufSmallRet,WinErr,BufSmallFill,GoodRet\n"));

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

