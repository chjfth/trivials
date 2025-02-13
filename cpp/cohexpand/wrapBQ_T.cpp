// This version is only used for MSVC, due to using the _T() macro.
// This is stramphibian code that can do Unicode-build or ANSI build
// to print-out the same text.

#include <stdio.h>
#include <tchar.h>
#include <locale.h>

#define cox_MAKE_STRING(asymbol) #asymbol
#define coy_MAKE_STRING(s) cox_MAKE_STRING(s)

#define WRAP_BQ(s) coy_MAKE_STRING(  [s]  )

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	const TCHAR *psz = _T( WRAP_BQ(greet) );
	_tprintf(_T("[T]Greeting: %s\n"), psz);
	return 0;
}
