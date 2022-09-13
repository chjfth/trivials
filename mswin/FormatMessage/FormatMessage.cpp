#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <tchar.h>
#include <locale.h>

#include "0409.h"
#include "0411.h"

#define VERSION _T("1.2")

// For the following 3, please/must keep them in accordance with those in .mc files.
#ifndef MSG_en_US_only
#define MSG_en_US_only 2000 
#endif
#define MSG_zh_CN_only 2001
#define MSG_zh_TW_only 2002

////////

#define LANGID_AutoSelect 0
#define LANGID_en_US 0x0409
#define LANGID_zh_CN 0x0804
#define LANGID_zh_TW 0x0404
#define LANGID_ja_JP 0x0411
#define LANGID_ru_RU 0x0419

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
		FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS, 
		NULL,   // lpSource
		winerr, // dwMessageId
		0,		// dwLanguageId
		s_errstr, ARRAYSIZE(s_errstr),
		NULL); // A trailing \r\n has been filled.

	if(retchars==0) // error
	{
		_sntprintf_s(s_errstr, _TRUNCATE, _T("Unknown Windows error code: %u"), winerr);
	}
	else
	{
		StrTrim(s_errstr, _T("\r\n"));
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
		_T("Shrink+Extend [  123][22334455]"),
		_T("Shrink+Extend [%1!*.*s!][%3!x!]"), 5,3, _T("1234"), 0x22334455
		); //              %*.*s

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
	DWORD_PTR pargs1[] = {-123, (DWORD_PTR)_T("ABC")};
	test_flag_ARGUMENT_ARRAY(
		_T("A 32-bit number -123 and a string ABC"),
		_T("A 32-bit number %1!d! and a string %2"), pargs1
		);

	// Now the example from MSDN:
	DWORD_PTR pArgs2[] = { 
		4, 2, (DWORD_PTR)L"Bill",  // %1!*.*s! refers back to the first insertion string in pMessage
		(DWORD_PTR)L"Bob",         // %4 refers back to the second insertion string in pMessage
		6, (DWORD_PTR)L"Bill" };   // %5!*s! refers back to the third insertion string in pMessage
	test_flag_ARGUMENT_ARRAY( 
		_T("  Bi Bob   Bill"),
		_T("%1!*.*s! %4 %5!*s!"), pArgs2
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

	TCHAR szLang[40]=_T("0=Auto-select");
	
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
		_tprintf(_T("FormatMessage() fail. WinErr=%d, %s\n"), 
			winerr, get_winerr_string(winerr));
		_tprintf(_T("\n"));
	}
}

void test_MessageFromDll()
{
	HMODULE hDll = LoadLibrary(_T("0411.dll")); // Japanese resource dll
	if(!hDll){
		_tprintf(_T("LoadLibrary(\"0411.dll\") fail!"));
		return;
	}

	test_MessageFromModule(hDll, MSG_JingYeSi, LANGID_ja_JP); // 0x0411

	FreeLibrary(hDll);
}

void tests_MessageFromModule()
{
	test_MessageFromModule(NULL, MSG_DengGuanQueLou, LANGID_en_US); // 0x0409 English 
	test_MessageFromModule(NULL, MSG_DengGuanQueLou, LANGID_zh_CN); // 0x0804 Simplified-Chinese.
	test_MessageFromModule(NULL, MSG_DengGuanQueLou, LANGID_ru_RU); // 0x0419 Russian, this will fail

	test_MessageFromDll();
}

void test_flag_IGNORE_INSERTS(HMODULE hmodule, DWORD msgid, DWORD langid)
{
	TCHAR textbuf[200] = {};

	int retchars = FormatMessage(
		FORMAT_MESSAGE_FROM_HMODULE|FORMAT_MESSAGE_IGNORE_INSERTS,
		hmodule, // lpSource£¬ NULL means from current EXE
		msgid, // dwMessageId
		langid,  // dwLanguageId
		textbuf, ARRAYSIZE(textbuf),
		NULL);

	assert(retchars>0);
	_tprintf(_T("FORMAT_MESSAGE_IGNORE_INSERTS got:\n  %s\n"), textbuf);

	assert_equal(_T("LocalDate is %1!04d!-%2!02d!-%3!02d!\r\n"), textbuf);
}

void test_expand_DATE(HMODULE hmodule, DWORD msgid, DWORD langid, 
	int year, int month , int day)
{
	(void)year, (void)month, (void)day; // they will be presented in va_list.
	TCHAR textbuf[200] = {};

	va_list args;
	va_start(args, langid);

	int retchars = FormatMessage(FORMAT_MESSAGE_FROM_HMODULE, 
		hmodule, msgid, langid, 
		textbuf, ARRAYSIZE(textbuf), &args);

	assert(retchars>0);
	_tprintf(_T("test_expand_INSERTS() got:\n  %s\n"), textbuf);

	va_end(args);
}

void tests_flag_IGNORE_INSERTS()
{
	test_flag_IGNORE_INSERTS(NULL, MSG_LocalDate, LANGID_en_US);

	SYSTEMTIME st = {};
	GetLocalTime(&st);
	test_expand_DATE(NULL, MSG_LocalDate, LANGID_en_US, 
		st.wYear, st.wMonth, st.wDay);

}

void test_LangID_auto_select()
{
	const int total = 4;

	const DWORD msgid_all_have = 1002;
	_tprintf(_T("[1/%d] Request MessageID=%u available in all of en-US,zh-CN,zh-TW\n"), 
		total, msgid_all_have);
	test_MessageFromModule(NULL, msgid_all_have, LANGID_AutoSelect);

	_tprintf(_T("[2/%d] Request MessageID=%u available in en-US only.\n"),
		total, MSG_en_US_only);
	test_MessageFromModule(NULL, MSG_en_US_only, LANGID_AutoSelect);

	_tprintf(_T("[3/%d] Request MessageID=%u available in zh-CN only.\n"),
		total, MSG_zh_CN_only); 
	test_MessageFromModule(NULL, MSG_zh_CN_only, LANGID_AutoSelect);

	_tprintf(_T("[4/%d] Request MessageID=%u available in zh-TW only.\n"),
		total, MSG_zh_TW_only); 
	test_MessageFromModule(NULL, MSG_zh_TW_only, LANGID_AutoSelect);
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	_tprintf(_T("WinAPI FormatMessage demo, v%s\n"), VERSION);
	
	_tprintf(_T("==== Test flag: FORMAT_MESSAGE_FROM_STRING\n"));
	tests_flag_FROM_STRING();
	_tprintf(_T("\n"));

	_tprintf(_T("==== Test flag: FORMAT_MESSAGE_ARGUMENT_ARRAY\n"));
	tests_flag_ARGUMENT_ARRAY();
	_tprintf(_T("\n"));

	_tprintf(_T("==== Test flag: FORMAT_MESSAGE_FROM_HMODULE\n"));
	tests_MessageFromModule();
	_tprintf(_T("\n"));

	_tprintf(_T("==== Test LangID auto-select.\n"));
	test_LangID_auto_select();
	_tprintf(_T("\n"));

	tests_flag_IGNORE_INSERTS();

	return 0;
}
