#undef NDEBUG // always enable assert()

// Define this from command-line to see TScalableArray<T>::Realloc() crash,
// on certain versions of `gcc -O2` (gcc 13.4 etc). (EvClip 20260520.c1)
//#define _XXX_SEE_REALLOC_CRASH 

#include <stdio.h>
#include <assert.h>
#include <locale.h>
#include <ps_TCHAR.h>
#include <commdefs.h>
#include <msvc_extras.h>
#include <cxx_stack.h>
	using namespace chjds;
#include <sdring.h>

#define LOCK_YES(stk) ((stk).dbg_peek_enumlocks()>0)
#define LOCK_NO(stk) ((stk).dbg_peek_enumlocks()==0)

void see_realloc_crash()
{
	// Create a custom stack, so that we can enumerate elements from that stack.

	TCHAR const* const artext[] = { _T("ele0"), _T("ele1"), _T("ele2") };
	const int ncount = ARRAY_SIZE(artext);
	int i, idx = 0;
	Sdring *ps = nullptr;

	stack<Sdring> stk1;
	for (i = 0; i < ncount; i++)
		stk1.push(artext[i]);

	printf("Constructing stk2 from stk1, START.\n");

	stack<Sdring> stk2 = stk1;

	printf("Constructing stk2 from stk1, DONE.\n");
}


int _tmain(int argc, TCHAR *argv[])
{
	setlocale(LC_ALL, "");

	see_realloc_crash();

	printf("//see_realloc_crash Success.//\n");
	return 0;
}
