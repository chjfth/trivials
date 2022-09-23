/*
 * This program applies _strupr() to some byte stream,
 * then applies _mbsupr() to the same byte stream.
 * We will then see different behavior of _strupr() and _mbsupr().
 * _strupr() goes with locale's locale-codepage(.lc_codepage) ;
 * _mbsupr() goes with locale's multibyte-codepage(.mbcodepage), set by _setmbcp().
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h> // ARRAYSIZE
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <mbstring.h>
#include <mbctype.h>
#include <fcntl.h>
#include <io.h>
#include <assert.h>

void printhex(const unsigned char s[], int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("%02x ", s[i]);
	}
	printf("\n");
}

void print_chars_glyph_by_codepage(int codepage,
	const unsigned char *psz_mbcs)
{
	WCHAR wbuf[200] = {}, wbuf2[600] = {};
	//	MultiByteToWideChar(codepage, 0, (char*)psz_mbcs, -1, wbuf, ARRAYSIZE(wbuf));

		// set WCHAR-fidelity mode for wprintf()
	_setmode(_fileno(stdout), _O_U8TEXT);

	int i;
	for (i = 0; psz_mbcs[i]; i++)
	{
		bool err = false;
		WCHAR wc = 0;
		int cret = MultiByteToWideChar(codepage, MB_ERR_INVALID_CHARS,
			(char*)(psz_mbcs + i), 1,
			&wc, 1);

		if (cret == 0)
			err = true;

		if (wc == 0)
			err = true;

		// Bug workaround: https://stackoverflow.com/q/73809381/151453
		if (wc >= 0x80 && wc <= 0x9F)
		{
			// '\x98' from cp2153 may reach this, and I think it's wrong behavior.
			err = true;
		}

		wbuf2[i * 3] = err ? ' ' : wc;

		wbuf2[i * 3 + 1] = ' ', wbuf2[i * 3 + 2] = ' ';
	}

	wbuf2[i * 3] = '\0';

	wprintf(L"%s\n", wbuf2);

	// restore narrow mode so that printf() can later be used
	_setmode(_fileno(stdout), _O_TEXT);
}

void test_toupper_cp737_greek(bool use_mbsupr)
{
	char *setlocret = setlocale(LC_CTYPE, ".1253");
	int setret = _setmbcp(737);

	int now_mbcp = _getmbcp();
	// -- will return 0 for SBCS(e.g. cp737), bad design from Microsoft.

	printf("Called setlocale(LC_CTYPE,\".1253\"); \n");
	printf("setlocale() returns: %s\n", setlocret);
	
	printf("Called _setmbcp(737), returns %s.\n", setret == 0 ? "success" : "fail");
	printf("_getmbcp() = %d (If 0, it tells nothing.)\n", now_mbcp);

	int i;
	unsigned char greeks1253[27] = {};
	for (i = 0; i < 26; i++)
		greeks1253[i] = 0xE0 + i; // fill with cp1253 small letters

	unsigned char greeks737[25] = {};
	for (i = 0; i < 24; i++)
		greeks737[i] = 0x98 + i; // fill with cp737 small letters

	int printchar_codepage = use_mbsupr ? 737 : 1253;
	const char *printchar_as = use_mbsupr ? "(as cp737)" : "(as cp1253)";

	/// test greeks1253[] chars

	printf("%20s: ", "Input");
	printhex(greeks1253, 26);
	printf("%20s: ", printchar_as);
	print_chars_glyph_by_codepage(printchar_codepage, greeks1253);

	if (use_mbsupr)
		_mbsupr_s(greeks1253);
	else
		_strupr_s((char*)greeks1253, sizeof(greeks1253)); // this gets lower-to-upper

	printf("%20s: ", use_mbsupr ? "_mbsupr()" : "_strupr()");
	printhex(greeks1253, 26);
	printf("%20s: ", printchar_as);
	print_chars_glyph_by_codepage(printchar_codepage, greeks1253);

	printf("\n");

	/// test greeks737[] chars

	printf("%20s: ", "Input");
	printhex(greeks737, 24);
	printf("%20s: ", printchar_as);
	print_chars_glyph_by_codepage(printchar_codepage, greeks737);

	if (use_mbsupr)
		_mbsupr_s(greeks737); // this gets lower-to-upper
	else
		_strupr_s((char*)greeks737, sizeof(greeks737));

	printf("%20s: ", use_mbsupr ? "_mbsupr()" : "_strupr()");
	printhex(greeks737, 24);
	printf("%20s: ", printchar_as);
	print_chars_glyph_by_codepage(printchar_codepage, greeks737);
}



void test_MultibyteToWideChar()
{
	WCHAR wbuf[10] = {};
	int ret = MultiByteToWideChar(1253, MB_ERR_INVALID_CHARS,
		"\x98\xE4\xFF\xE4", 5,
		wbuf, 10);
}

int _tmain(int argc, TCHAR* argv[])
{
	//	test_MultibyteToWideChar();

	//	setlocale(LC_ALL, "");

	test_toupper_cp737_greek(false);
	printf("\n");
	test_toupper_cp737_greek(true);

}
