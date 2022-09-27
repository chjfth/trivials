/*
	This program demonstrates, what would be different before/after we call
	MSVCRT setloacale(LC_ALL, ""); .
*/
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

WCHAR *HexdumpW(const WCHAR *pszw, int count, WCHAR *hexbuf, int bufchars)
{
	if (count < 0)
		count = wcslen(pszw);

	for (int i = 0; i < count; i++)
	{
		_snwprintf_s(hexbuf + i * 5, bufchars - i * 5, _TRUNCATE,
			L"%04X ", (unsigned short)pszw[i]);
	}

	int wlen = (int)wcslen(hexbuf);
	if (wlen > 0 && hexbuf[wlen - 1] == L' ')
		hexbuf[wlen - 1] = L'\0';

	return hexbuf;
}

char *HexdumpA(const char *pbytes, int count, char *hexbuf, int bufchars)
{
	if (count < 0)
		count = strlen(pbytes);

	for (int i = 0; i < count; i++)
	{
		_snprintf_s(hexbuf + i * 3, bufchars - i * 3, _TRUNCATE,
			"%02X ", (unsigned char)pbytes[i]);
	}

	int alen = (int)strlen(hexbuf);
	if (alen > 0 && hexbuf[alen - 1] == ' ')
		hexbuf[alen - 1] = L'\0';

	return hexbuf;
}


void print_sample()
{
	const char pansi[] = "\xB5\xE7"; // '电' in GBK, '萇' in Big5
	printf("printf: %s\n", pansi);

	errno_t err = 0;
	char szerr[80];
	size_t retlen = 0; // will be TCHARs in output buffer
	WCHAR wcbuf[20] = {}, hexbufw[60] = {};
	err = mbstowcs_s(&retlen, wcbuf, pansi, sizeof(pansi));
	if(!err)
	{
		printf("  mbstowcs_s() result:\n");
		wprintf(L"  # %s\n", HexdumpW(wcbuf, -1, hexbufw, ARRAYSIZE(hexbufw)));
		wprintf(L"  > %s\n", wcbuf);
	}
	else
	{
		strerror_s(szerr, err);
		printf("  mbstowcs_s() error! errno=%d, %s\n", err, szerr);
	}

	printf("\n");

	const WCHAR ustr[] = L"\x99AC"; // '馬'
	wprintf(L"wprintf: %s\n", ustr);

	char ansibuf[20] = {}, hexbufa[60];
	err = wcstombs_s(&retlen, ansibuf, ustr, ARRAYSIZE(ustr));
	if(!err)
	{
		printf("  wcstombs_s() result:\n");
		printf("  # %s\n", HexdumpA(ansibuf, -1, hexbufa, ARRAYSIZE(hexbufa)));
		printf("  > %s\n", ansibuf);
	}
	else
	{
		strerror_s(szerr, err);
		printf("  wcstombs_s() error! errno=%d, %s\n", err, szerr);
		// -- probably EILSEQ(42)
	}

	printf("\n");
}

int _tmain(int argc, TCHAR* argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);

	printf("Before setlocale(LC_ALL, \"\");\n");
	print_sample();

	setlocale(LC_ALL, "");

	printf("After setlocale(LC_ALL, \"\");\n");
	print_sample();
	
	return 0;
}

