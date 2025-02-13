#include <stdio.h>
#include <locale.h>
#include <wchar.h>

// This code can be compiled with MSVC and Linux-gcc.

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

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	
	printf("[A]greet: %s\n", s_greet);

#ifdef _WIN32
	wprintf(L"[w]greet: %s\n", sw_greet); // small %s for wide-string
#else
	// Reopen stdout in wide mode, Linux gcc requires this.
	freopen(NULL, "w", stdout);
	wprintf(L"[W]greet: %S\n", sw_greet); // big %s for wide-string
#endif

	return 0;
}

