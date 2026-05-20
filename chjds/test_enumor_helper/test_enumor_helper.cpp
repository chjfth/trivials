#undef NDEBUG // always enable assert()

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

void test_enumor_helper()
{
	// Create a custom stack, so that we can enumerate elements from that stack.

	TCHAR const* const artext[] = {_T("ele0"), _T("ele1"), _T("ele2") };
	const int ncount = ARRAY_SIZE(artext);
	int i, idx=0;
	Sdring *ps = nullptr;

	stack<Sdring> stk1;
	for(i=0; i<ncount; i++)
		stk1.push( artext[i] );

	stack<Sdring> stk2 = stk1;

	auto en1 = stk1.get_enumor();
	assert(LOCK_NO(stk1));

	for(i=0; ;i++)
	{
		idx = en1.next(&ps);
		if(idx==-1)
			break;

		assert( idx == i );
		assert( Sdring::str_match(artext[i], ps->c_str()) );

		assert(LOCK_YES(stk1));
	}
	assert(i == ncount);
	assert(LOCK_NO(stk1));

	//
	// Test about reset() and lock-state.
	//

	en1.reset();
	assert(LOCK_NO(stk1));

	idx = en1.next(&ps);
	assert(idx==0 && Sdring::str_match(artext[0], ps->c_str()));
	assert(LOCK_YES(stk1));

	// We can reset() in midway, and lock-state is free-d.
	en1.reset();
	assert(LOCK_NO(stk1));

	//
	// Verify again.
	//
	for(i=0; ;i++)
	{
		idx = en1.next(&ps);
		if (idx == -1)
			break;

		assert(idx == i);
		assert(Sdring::str_match(artext[i], ps->c_str()));

		assert(LOCK_YES(stk1));
	}
	assert(i == ncount);
	assert(LOCK_NO(stk1));
}


int _tmain(int argc, TCHAR *argv[])
{
	setlocale(LC_ALL, "");

#ifdef _MSC_VER
	// Take snapshot before operations
	_CrtMemState state1 = {}, state2 = {}, stateDiff = {};
	_CrtMemCheckpoint(&state1);
#endif


	test_enumor_helper();

#ifdef _MSC_VER
	// Take snapshot after
	_CrtMemCheckpoint(&state2);

	// Compare snapshots
	if (_CrtMemDifference(&stateDiff, &state1, &state2))
	{
		_CrtMemDumpStatistics(&stateDiff);
		_CrtMemDumpAllObjectsSince(&state1);
		_tprintf(_T("Memory leak detected!!! See debug channel for details.\n"));

		return 4;
	}
#endif // _MSC_VER

	printf("Success.\n");
	return 0;
}
