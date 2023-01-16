#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <winnls.h>

void my_tprintf(const TCHAR* szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);

	TCHAR buf[2000] = {};
	_vsntprintf_s(buf, ARRAYSIZE(buf), szfmt, args);

	va_end(args);

	_tprintf(_T("%s"), buf);
	fflush(stdout); // important
}

const TCHAR* app_GetWindowsVersionStr3()
{
	typedef DWORD __stdcall PROC_RtlGetVersion(OSVERSIONINFOEX*);
	typedef BOOL __stdcall PROC_GetVersionEx(OSVERSIONINFOEX*);

	static TCHAR s_verstr[40];
	OSVERSIONINFOEX exver = { sizeof(OSVERSIONINFOEX) };

	PROC_RtlGetVersion* RtlGetVersion = (PROC_RtlGetVersion*)
		GetProcAddress(GetModuleHandle(_T("ntdll")), "RtlGetVersion");

	PROC_GetVersionEx* dllGetVersionEx = (PROC_GetVersionEx*)
		GetProcAddress(GetModuleHandle(_T("kernel32")), "GetVersionEx");

	if (RtlGetVersion)
		RtlGetVersion(&exver);

	if (exver.dwMajorVersion == 0)
	{
		// RtlGetVersion() fail, fall back to traditional GetVersionEx()
		BOOL succ = dllGetVersionEx && dllGetVersionEx(&exver);
		if (!succ)
			exver.dwMajorVersion = 0;
	}

	if (exver.dwMajorVersion > 0)
	{
		_sntprintf_s(s_verstr, ARRAYSIZE(s_verstr), _T("%d.%d.%d"),
			exver.dwMajorVersion, exver.dwMinorVersion, exver.dwBuildNumber);
	}
	else
	{
		_sntprintf_s(s_verstr, ARRAYSIZE(s_verstr), _T("%s"),
			_T("Fail to get Windows OS version after trying NTDLL!RtlGetVersion() and GetVersionEx()!"));
	}

	return s_verstr;
}

void app_print_version(const TCHAR* verstr)
{
	my_tprintf(_T("Compiled at %s with _MSC_VER=%d (v%s)\n"), _T(__DATE__), _MSC_VER, verstr);
	my_tprintf(_T("This Windows OS version: %s\n"), app_GetWindowsVersionStr3());
	my_tprintf(_T("\n"));

}

//#if sizeof(TCHAR) == 1 // invalid integer constant expression
#ifndef UNICODE
typedef unsigned char UTCHAR;
#else
typedef TCHAR UTCHAR;
#endif

void print_hexdump(const TCHAR* pszText)
{
	int Textlen = _tcslen(pszText);
	int i;
	for (i = 0; i < Textlen; i++)
	{
		TCHAR c = (UTCHAR)pszText[i];
		if(c<256)
			_tprintf(_T("%02X "), c);
		else
			_tprintf(_T("%04X "), c);
	}
}

void concat_strings(TCHAR *obuf, int obufsize, ...)
{
	va_list args;
	va_start(args, obufsize);

	for (; ;)
	{
		const TCHAR* str = va_arg(args, TCHAR*);
		if (!str)
			break;
		
		_tcscat_s(obuf, obufsize, str);
	}
}

void print_one_case(const TCHAR *prefix, const TCHAR *lang, 
	const TCHAR *spestr, const TCHAR *country, const TCHAR* tailstr=nullptr)
{
	TCHAR szFinal[200] = {};
	concat_strings(szFinal, 200, prefix, lang, spestr, country, tailstr, nullptr);
	my_tprintf(_T("%s\n"), szFinal);
	print_hexdump(szFinal);
	my_tprintf(_T("\n\n"));
}

void print_arab(LANGID langid, const TCHAR *sepstr, const TCHAR* tailstr)
{
	TCHAR szLang[100] = {}, szCountry[100] = {};
	TCHAR szFinal1[200] = {} , szFinal2[200] = {};
	
	GetLocaleInfo(langid, LOCALE_SNATIVELANGNAME, szLang, ARRAYSIZE(szLang)-1);
	GetLocaleInfo(langid, LOCALE_SNATIVECOUNTRYNAME, szCountry, ARRAYSIZE(szCountry)-1);

	if(!sepstr)
	{
		print_one_case(_T("CASE1: "), szLang, _T(" spkin "), szCountry); // spoken in
		print_one_case(_T("CASE2: "), szLang, _T(" @ "), szCountry);

		my_tprintf(_T("Hint: Run `print-arab 0x0401 at` to customize params.\n"));
	}
	else
	{
		print_one_case(_T("CUST:"), szLang, sepstr, szCountry, tailstr);
		print_one_case(_T(""), szLang, sepstr, szCountry, tailstr);
	}
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	_setmode(_fileno(stdout), _O_U8TEXT); // let CRT preserve wide char

	LANGID langid = 0x0C01; // Arabic @ Egypt
	const TCHAR* sepstr = nullptr;
	const TCHAR* tailstr = nullptr;

	if(argc>1)
		langid = (LANGID)_tcstoul(argv[1], nullptr, 0);

	if(argc>2)
		sepstr = argv[2];

	if (argc > 3)
		tailstr = argv[3];

	TCHAR szLangidDesc[100] = {};
	GetLocaleInfo(langid, LOCALE_SLOCALIZEDDISPLAYNAME, szLangidDesc, ARRAYSIZE(szLangidDesc));
	
	my_tprintf(_T("Using LANGID = 0x%04X , %s\n\n"), langid, szLangidDesc);

	print_arab(langid, sepstr, tailstr);

	return 0;
}

