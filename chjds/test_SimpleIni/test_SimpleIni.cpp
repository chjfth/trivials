#include "CHHI_DEBUG.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ps_TCHAR.h>
#include <locale.h>
#include <utility> // On MSVC, this brings in _CrtMemState()
#include <msvc_extras.h>
#include <sdring.h>

#include "SimpleIni.h"

void do_test1()
{
// 	Sdring sz1(_T("ABC"));
// 	int kk = sz1;

	const TCHAR *inifilename = _T("sample1.ini");

	_tprintf(_T("Reading INI file: %s\n\n"), inifilename);

	SimpleIni ini;
	SimpleIni::ReCode_et err = ini.read(inifilename);
	assert(!err);

	int i;
	const TCHAR *secname = nullptr;

	Sdrings section_names = ini.sections();
	for(i=0; i<section_names.count(); i++)
	{
		secname = section_names[i];
		_tprintf(_T("Section#%d [%s]\n"), i+1, secname);

		Sdrings seckeys = ini.section_keys(secname);
		for(int j=0; j<seckeys.count(); j++)
		{
			const TCHAR *keyname = seckeys[j];
			_tprintf(_T("'%s' = \n"), keyname);
		}

		_tprintf(_T("\n"));
	}
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

#ifdef _MSC_VER
	// Take snapshot before operations
	_CrtMemState state1 = {}, state2 = {}, stateDiff = {};
	_CrtMemCheckpoint(&state1);
#endif

	do_test1();

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

