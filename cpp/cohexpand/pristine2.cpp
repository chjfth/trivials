#include <stdio.h>
#include <wchar.h>

// Compared to pristine1.cpp, this version applies WRAP_BQ()'s result to wide-string.

#define cox_MAKE_STRING(asymbol) #asymbol
#define coy_MAKE_STRING(s) cox_MAKE_STRING(s)

#define WRAP_B(s) [s]
#define WRAP_BQ(s) coy_MAKE_STRING( WRAP_B(s) )

const char *s_greet = WRAP_BQ(greet);

////

#define cox_MAKE_WIDE(astring) L ## astring
#define coy_MAKE_WIDE(s) cox_MAKE_WIDE(s)
const wchar_t *sw_greet = coy_MAKE_WIDE( WRAP_BQ(greet) );


int main()
{
	printf("[A]Greet: %s\n", s_greet);

#ifdef _WIN32
	wprintf(L"[w]Greet: %s\n", sw_greet); // MSVCRT: %s for wprintf's wide-string
#else
	// Reopen stdout in wide mode, Linux gcc requires this.
	freopen(NULL, "w", stdout);
	wprintf(L"[W]Greet: %ls\n", sw_greet); // Glibc: %ls for wide-string
#endif

	return 0;
}
