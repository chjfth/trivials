// Compare to wrapBQ_T.cpp, this version wraps with extra starting & ending space-chars,
// implemented as WRAP_BSQ() macro.
// Starting & ending Space-chars are defaultly stripped off from macro arguments,
// so some special treatment is required. (still feeling weird)

#include <stdio.h>
#include <tchar.h>
#include <locale.h>

#define cox_MAKE_STRING(asymbol) #asymbol
#define coy_MAKE_STRING(s) cox_MAKE_STRING(s)

#define NN
#define WW NN NN

#define WRAP_BS(s) [s]

#define WRAP_BSQ(s) coy_MAKE_STRING(NN WRAP_BS(s) WW)
// -- Now we get " [greet] ".

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	const TCHAR *psz = _T(WRAP_BSQ(greet));
	_tprintf(_T("[T]Greeting: {%s}\n"), psz);
	return 0;
}
