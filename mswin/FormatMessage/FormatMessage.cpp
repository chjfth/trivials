#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <tchar.h>
#include <locale.h>

#include "0409.h"

void assert_equal(const TCHAR *s1correct, const TCHAR *s2tocheck)
{
	static int s_count = 0;
	s_count++;

	_tprintf(_T("[%d] Verifying: %s\n"), s_count, s1correct);

	if( _tcscmp(s1correct, s2tocheck)!=0 )
	{
		_tprintf(_T("   Error! Got: %s\n"), s2tocheck);
	}

	assert( _tcscmp(s1correct, s2tocheck)==0 );
}

const TCHAR *get_winerr_string(DWORD winerr)
{
	static TCHAR s_errstr[400];
	s_errstr[0] = 0;

	DWORD retchars = FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM, 
		NULL,   // lpSource
		winerr, // dwMessageId
		0,		// dwLanguageId
		s_errstr, ARRAYSIZE(s_errstr),
		NULL); // A trailing \r\n has been filled.

	if(retchars==0) // error
	{
		_sntprintf_s(s_errstr, _TRUNCATE, _T("Unknown Windows error code: %u"), winerr);
	}

	return s_errstr;
}

void test_flag_FROM_STRING(
	const TCHAR *sz_verify_result,
	const TCHAR *szfmt, ...
	)
{
	TCHAR textbuf[200] = {};

	va_list args;
	va_start(args, szfmt);

	int retchars = FormatMessage(
		FORMAT_MESSAGE_FROM_STRING,  // dwFlags
		szfmt, // lpSource
		0, // dwMessageId  (no-use)
		0, // dwLanguageId (no-use)
		textbuf,
		ARRAYSIZE(textbuf),
		&args);

	assert(retchars>0);
	assert_equal(sz_verify_result, textbuf);

	va_end(args);
}

void tests_flag_FROM_STRING()
{
	test_flag_FROM_STRING(
		_T("Parameter ONE before TWO"),
		_T("Parameter %1 before %2"), _T("ONE"), _T("TWO")
		);

	test_flag_FROM_STRING(
		_T("Parameter TWO after ONE"),
		_T("Parameter %2 after %1"), _T("ONE"), _T("TWO")
		);

	test_flag_FROM_STRING(
		_T("Use %1 twice: ONE, TWO, ONE !"),
		_T("Use %%1 twice: %1, %2, %1 !"), _T("ONE"), _T("TWO"), _T("no use")
		);

	test_flag_FROM_STRING(
		_T("Extend to 5 chars align left [123  ]"),
		_T("Extend to 5 chars align left [%1!-*.*s!]"), 5, 5, _T("123")
		);  //                            %-*.*s

	test_flag_FROM_STRING(
		_T("Shrink to 5 chars [12345]"),
		_T("Shrink to 5 chars [%1!*.*s!]"), 5, 5, _T("123456")
		); //                  %*.*s

	test_flag_FROM_STRING(
		_T("Format from numbers 510==0x01FE"),
		_T("Format from numbers %1!u!==0x%2!04X!"), 510, 510
		); //                   %u       %04X
}

void test_flag_ARGUMENT_ARRAY(
	const TCHAR *sz_verify_result,
	const TCHAR *szfmt, 
	DWORD_PTR pargs[])
{
	TCHAR textbuf[200] = {};

	int retchars = FormatMessage(
		FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,  // dwFlags
		szfmt, // lpSource
		0, // dwMessageId  (no-use)
		0, // dwLanguageId (no-use)
		textbuf,
		ARRAYSIZE(textbuf),
		(va_list*)pargs);

	assert(retchars>0);
	assert_equal(sz_verify_result, textbuf);
}

void tests_flag_ARGUMENT_ARRAY()
{
	DWORD_PTR pargs[] = {-123, (DWORD_PTR)_T("ABC")};
	test_flag_ARGUMENT_ARRAY(
		_T("A 32-bit number -123 and a string ABC"),
		_T("A 32-bit number %1!d! and a string %2"), pargs
		);
}

void test_MessageFromModule(HMODULE hmodule, DWORD msgid, DWORD langid, ...)
{
	// ERROR_RESOURCE_TYPE_NOT_FOUND    1813L

	TCHAR textbuf[200];
	va_list args;
	va_start(args, langid);

	int retchars = FormatMessage(
		FORMAT_MESSAGE_FROM_HMODULE, 
		hmodule, // lpSource: the HMODULE of an EXE or DLL
		msgid,   // dwMessageId
		langid,  // dwLanguageId, 0 to auto-select
		textbuf, 
		ARRAYSIZE(textbuf),
		(va_list*)&args);

	va_end(args);

	TCHAR szLang[40]=_T("?");
	
	if(langid!=0)
	{
		GetLocaleInfo(langid, LOCALE_SLOCALIZEDDISPLAYNAME, szLang, ARRAYSIZE(szLang));
	}

	_tprintf(_T("hmodule=0x%X, msgid=%d, langid=0x%04X [%s]:\n"),
		hmodule, msgid, langid, szLang);
	if(retchars>0)
	{
		_tprintf(_T("%s\n"), textbuf);
	}
	else
	{
		DWORD winerr = GetLastError();
		_tprintf(_T("FormateMessage() fail. WinErr=%d, %s\n"), 
			winerr, get_winerr_string(winerr));
	}
}

void tests_MessageFromModule()
{
	test_MessageFromModule(NULL, MSG_DengGuanQueLou, 0x0409); // English.
	test_MessageFromModule(NULL, MSG_DengGuanQueLou, 0x0804); // Simplified-Chinese.
	test_MessageFromModule(NULL, MSG_DengGuanQueLou, 0x0419); // Russian, this will fail
	test_MessageFromModule(NULL, MSG_DengGuanQueLou, 0); // auto-select

	HMODULE hDll = LoadLibrary(_T("0404.dll")); // the Traditional-Chinese resource dll
	if(!hDll){
		_tprintf(_T("LoadLibrary(\"0404.dll\") fail!"));
		return;
	}

	test_MessageFromModule(hDll, MSG_DengGuanQueLou, 0x0404);

	FreeLibrary(hDll);
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	_tprintf(_T("==== Test flag: FORMAT_MESSAGE_FROM_STRING\n"));
	tests_flag_FROM_STRING();
	_tprintf(_T("\n"));

	_tprintf(_T("==== Test flag: FORMAT_MESSAGE_ARGUMENT_ARRAY\n"));
	tests_flag_ARGUMENT_ARRAY();
	_tprintf(_T("\n"));

	_tprintf(_T("==== Test flag: FORMAT_MESSAGE_FROM_HMODULE\n"));
	tests_MessageFromModule();
	_tprintf(_T("\n"));

	return 0;
}

