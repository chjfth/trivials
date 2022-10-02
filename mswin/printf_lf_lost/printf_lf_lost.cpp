#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

void printf_lf_lost()
{
	const char* utf8seq = "\xE7\x94\xB5";

	printf("Line 1: %s", utf8seq);
	printf("\n"); // "Line 1" and this \n will be overwritten.

	printf("Line 2.\n");

	printf("Line 3: %s", utf8seq);
	printf(" \n"); // Thanks to the space-char, \n is preserved.
	printf("Line 4.\n");
}

int _tmain(int argc, TCHAR* argv[])
{
	// setlocale is required, to see the "LF lost" symptom
	const char *locret = setlocale(LC_ALL, "");

	printf("Compiled with _MSC_VER=%d\n", _MSC_VER);

	printf("setlocale(LC_ALL, \"\"); returns:\n");
	printf("> %s\n", locret);
	printf("GetConsoleOutputCP() = %d\n", GetConsoleOutputCP());

//	int err = setvbuf(stdout, NULL, _IONBF, 0); // does not matter

	int conscp = GetConsoleOutputCP();
	if(conscp==932 || conscp==936 || conscp==949 || conscp==950)
	{
		// OK
	}
	else
	{
		printf("[ERROR] In order to see the symptom, you should set system-ANSI-codepage to one of:\n");
		printf("    932(ja-JP), 936(zh-CN), 949(ko-KR) or 950(zh-TW).\n");
		exit(1);
	}

	printf_lf_lost();

	return 0;
}
