#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <tchar.h>
#include <locale.h>

#ifdef _MSC_VER
void print_with_wcsftime(const struct tm &tm1)
{
	wprintf(L"Use MSVCRT wcsftime() below:\n");

	wchar_t wbuf1[80] = {};
	wcsftime(wbuf1, ARRAYSIZE(wbuf1), L"%x (%A); %X ; %z(%Z)", &tm1);

	// We use WriteConsoleW() instead of wprintf(), bcz wprintf() may ruin WCHARs.
	HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	int slen = wcslen(wbuf1);
	DWORD nwritten = 0;
	BOOL succ = WriteConsoleW(hStdout, wbuf1, slen, &nwritten, NULL);
	if(!succ)
	{
		// If we redirect CRT stdout, WriteConsoleW() will fail.
		printf("WriteConsoleW() fails, WinErr=%d\n", GetLastError());
	}
}
#endif

void print_help()
{
	const char * helpstr = 
"Param 1: The string passed to setlocale().If \"-\", not calling setlocale().\n"
"Param 2: If on Windows, pass this value to SetThreadUILanguage().\n"
"Example:\n"
"    strftime1 ja-JP 0x0404\n"
	;
	printf("%s", helpstr);
}

int main(int argc, char* argv[])
{
	if(argc==2 && 
		( strcmp(argv[1], "?")==0 || (argv[1], "-?")==0 )
		)
	{
		print_help();
		exit(1);
	}
		
	const char* locstr = NULL;
	if (argc == 1)
		locstr = "";
	else
	{
		if (argv[1][0] != '-')
			locstr = argv[1];
	}

	short uilang_to_set = 0;
	if(argc==3)
	{
		uilang_to_set = (short)strtoul(argv[2], NULL, 0);
	}

#ifdef _MSC_VER
	// We call SetThreadUILanguage() as early as *before* setlocale(),
	// so that strftime(%Z) is affected by SetThreadUILanguage().
	if (uilang_to_set != 0)
	{
		LANGID uilang_ret = SetThreadUILanguage(uilang_to_set);
		printf("SetThreadUILanguage(0x%04X) returns 0x%04X\n", uilang_to_set, uilang_ret);
	}
#endif

	if (locstr)
	{
		char* locret = setlocale(LC_ALL, locstr);
		if (locret)
			printf("setlocale(LC_ALL, \"%s\") returns: %s\n", locstr, locret);
		else
		{
			printf("setlocale(LC_ALL, \"%s\") ERROR!\n", locstr);
			exit(1);
		}
	}
	
	struct tm tm1 = {};
	tm1.tm_year = 2022 - 1900, tm1.tm_mon = 9 - 1, tm1.tm_mday = 17;
	tm1.tm_hour = 9, tm1.tm_min = 17, tm1.tm_sec = 3;
	tm1.tm_wday = 1;

	char buf1[80] = {};
	strftime(buf1, sizeof(buf1), "%x (%A); %X ; %z(%Z)", &tm1);

	char buf2[80] = {};
#ifdef _MSC_VER
	strftime(buf2, sizeof(buf2), "%#c", &tm1);
#else
	strftime(buf2, sizeof(buf2), "%c", &tm1);
#endif

	printf("%s\n", buf1);
	printf("%s\n", buf2);

#ifdef _MSC_VER
	printf(".\n");
	print_with_wcsftime( tm1);
#endif

	printf("\n");
	return 0;
}
