// Compare to wrapBQ_T.cpp, this version wraps with extra starting & ending space-chars,
// implemented as WRAP_BSQ1(), WRAP_BSQ2(), WRAP_BSQ3() macro.
// Starting & ending Space-chars are defaultly stripped off from macro arguments,
// so some special treatment is required. (still mysterious)

#include <stdio.h>
#include <tchar.h>
#include <locale.h>

#define cox_MAKE_STRING(asymbol) #asymbol
#define coy_MAKE_STRING(s) cox_MAKE_STRING(s)

#define NN
#define WW NN NN

#define WRAP_BS1(s) NN [s] WW // note: the trailing must be WW, not NN
#define WRAP_BS2(s) NN WRAP_BS1(s) NN
#define WRAP_BS3(s) NN WRAP_BS2(s) NN

#define WRAP_BSQ1(s) coy_MAKE_STRING(WRAP_BS1(s)) // We get " [greet] "

#define WRAP_BSQ2(s) coy_MAKE_STRING(WRAP_BS2(s))  // We get "  [greet]  "

#define WRAP_BSQ3(s) coy_MAKE_STRING(WRAP_BS3(s))  // We get "   [greet]   "

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	const TCHAR *ps1 = _T(WRAP_BSQ1(greet));
	_tprintf(_T("[T1]Greeting: {%s}\n"), ps1);

	const TCHAR *ps2 = _T(WRAP_BSQ2(greet));
	_tprintf(_T("[T2]Greeting: {%s}\n"), ps2);

	const TCHAR *ps3 = _T(WRAP_BSQ3(greet));
	_tprintf(_T("[T3]Greeting: {%s}\n"), ps3);
	return 0;
}
