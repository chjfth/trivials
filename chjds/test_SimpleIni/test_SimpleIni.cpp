#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ps_TCHAR.h>
#include <locale.h>
#include <utility> // On MSVC, this brings in _CrtMemState()
#include <msvc_extras.h>

#define NDEBUG // always enable assert()

#include "CHHI_DEBUG.h"

#include "SimpleIni.h"

void do_test()
{
	SimpleIni ini;
	SimpleIni::ReCode_et err = ini.read(_T("sample1.ini"));
	assert(!err);
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

#ifdef _MSC_VER
	// Take snapshot before operations
	_CrtMemState state1 = {}, state2 = {}, stateDiff = {};
	_CrtMemCheckpoint(&state1);
#endif

	do_test();

#ifdef _MSC_VER
	// Take snapshot after
	_CrtMemCheckpoint(&state2);

	// Compare snapshots
	if (_CrtMemDifference(&stateDiff, &state1, &state2))
	{
		_tprintf(_T("Memory leak detected!!! See debug channel for details.\n"));
		_CrtMemDumpStatistics(&stateDiff);
		_CrtMemDumpAllObjectsSince(&state1);

		return 4;
	}
#endif // _MSC_VER

	return 0;
}

