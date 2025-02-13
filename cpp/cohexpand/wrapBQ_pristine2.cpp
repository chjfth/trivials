#include <stdio.h>
#include <wchar.h>

// This is the stripped-off(clean) version of wrapBQ.cpp

#define cox_MAKE_STRING(asymbol) #asymbol
#define MAKE_STRING(s) cox_MAKE_STRING(s)

#define WRAP_B(s) [s]
#define WRAP_BQ(s) MAKE_STRING( WRAP_B(s) )

///

#define cox_MAKE_WIDE(astring) L ## astring
#define MAKE_WIDE(s) cox_MAKE_WIDE(s)
const wchar_t *sw_greet = MAKE_WIDE( WRAP_BQ(greet) );

const char *s_greet = WRAP_BQ(greet);

int main(int argc, char* argv[])
{
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

