#include <stdio.h>
#include <locale.h>
#include <wchar.h>

/*
wrapBQ: wrap a given token with brackets and quotes.
Input token:           greet
WRAP_BQ macro output:  "[greet]"

This code can be compiled with MSVC and Linux-gcc.

Compile with MSVC:

	cl.exe wrapBQ.cpp
	cl.exe /D greet=Hello wrapBQ.cpp

Compile with gcc:

	gcc -D greet="How do    you do?" wrapBQ.cpp

Example output:
	$ ./a.out
	[A]greet: [How do you do?]
	[W]greet: [How do you do?]
*/

#define cox_MAKE_STRING(asymbol) #asymbol
#define cox_MAKE_WSTRING(asymbol) L ## #asymbol

#define cox_MAKE_WIDE(astring) L ## astring
// -- User passed in astring argument should have been quoted in double-quotes.

#define MAKE_WIDE(s) cox_MAKE_WIDE(s)

#define WRAP_B(s) [s]

#define MAKE_STRING(s) cox_MAKE_STRING(s)
#define MAKE_WSTRING(s) cox_MAKE_WSTRING(s)

#define WRAP_BQ(s) MAKE_STRING( WRAP_B(s) )

const char *s_greet = WRAP_BQ(greet);
const wchar_t *sw_greet = MAKE_WIDE( WRAP_BQ(greet) );

int main()
{
	setlocale(LC_ALL, "");
	
	printf("[A]Greet: %s\n", s_greet);

#ifdef _WIN32
	wprintf(L"[w]Greet: %s\n", sw_greet); // MSVCRT: %s for wprintf's wide-string
#else
	// Reopen stdout in wide mode, Linux gcc requires this.
	freopen(NULL, "w", stdout);
	wprintf(L"[W]Greet: %ls\n", sw_greet);  // Glibc: %ls for wide-string
#endif

	return 0;
}

