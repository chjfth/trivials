#include <stdio.h>
#include <assert.h>
#include <locale.h>
#include <ps_TCHAR.h>
#include <msvc_extras.h>
#include <cxx_stack.h>
	using namespace chjds;
#include <Sdring.h>


void test0()
{
	// Create a custom stack, so that we can enumerate elements from that stack.

	stack<Sdring> stk1;
	stk1.push(_T("ele0"));
	stk1.push(_T("ele1"));
	stk1.push(_T("ele2"));

}


int _tmain(int argc, TCHAR *argv[])
{
	setlocale(LC_ALL, "");

#ifdef _MSC_VER
	// Take snapshot before operations
	_CrtMemState state1 = {}, state2 = {}, stateDiff = {};
	_CrtMemCheckpoint(&state1);
#endif


	test0();

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
