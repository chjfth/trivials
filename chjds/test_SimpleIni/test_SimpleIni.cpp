#include "CHHI_DEBUG.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ps_TCHAR.h>
#include <locale.h>
#include <utility> // On MSVC, this brings in _CrtMemState()
#include <msvc_extras.h>
#include <snTprintf.h>
#include <sdring.h>
#include <makeTsdring.h>
//#include <TScalableArray.h>

#include "SimpleIni.h"

bool IsEqual(const TCHAR *s1, const TCHAR *s2)
{
	return _tcscmp(s1, s2)==0 ? true : false;
}

void do_test1()
{
// 	Sdring sz1(_T("ABC"));
// 	int kk = sz1;

	TCHAR sbuf[800] = {};
	sdring<wchar_t> bigstrW; // in order to do exact compare no matter on Unicode or ANSI build

	const TCHAR *inifilename = _T("sample1.ini");

	_tprintf(_T("Reading INI file: %s\n\n"), inifilename);

	SimpleIni ini;
	SimpleIni::ReCode_et err = ini.read(inifilename);
	assert(!err);

	int i;
	const TCHAR *secname = nullptr;

	//
	// Test ini.sections() & section_keys().
	// Enumerate all sections, and for each section, enumerate all key-value pairs.
	//

	Sdring bigstr;

	Sdrings section_names = ini.sections();
	for(i=0; i<section_names.count(); i++)
	{
		secname = section_names[i];

		snTprintf(sbuf, _T("Section#%d [%s]\n"), i+1, secname);
		bigstr.append_self(sbuf);
		_tprintf(_T("%s"), sbuf);

		Sdrings seckeys = ini.section_keys(secname);
		for(int j=0; j<seckeys.count(); j++)
		{
			const TCHAR *keyname = seckeys[j];

			Sdring sval = ini.get(secname, keyname);

			snTprintf(sbuf, _T("'%s' = '%s'\n"), keyname, sval.getptr());
			bigstr.append_self(sbuf);
			_tprintf(_T("%s"), sbuf);
		}

		// Add an extra line between sections.
		bigstr.append_self(_T("\n"));
		_tprintf(_T("\n"));
	}

	bigstrW = makeWsdring(bigstr, mTs_UTF8);

	const wchar_t *answerW = L"\
Section#1 [global]\n\
'basedir' = 'dirFoo'\n\
\n\
Section#2 [section1]\n\
'key1' = '11'\n\
'key2' = '12'\n\
'keym' = 'val first line\n\
val second line'\n\
\n\
Section#3 [section2]\n\
'key1' = '21'\n\
'key2' = '22'\n\
'keym' = '\n\
val line one\n\
val line two\n\
val line three'\n\
\n\
Section#4 [unicodes]\n\
'key1' = 'ABC\x7535'\n\
\n"; //Note: \x7535 is a Unicode code-point(GBK dian).
	assert( sdring<wchar_t>::str_match(bigstrW, answerW) );

	assert( ini.has_key(_T("section1"), _T("key2")) );
	assert(!ini.has_key(_T("section1"), _T("keyZ")) );

	Sdring val;
	const TCHAR *sverify = _T("\n")
		_T("val line one\n")
		_T("val line two\n")
		_T("val line three");

	val = ini.get(_T("section2"), _T("keym"));
	assert( IsEqual(val, sverify) );

	val = ini.get(_T("section0"), _T("foo"));
	assert(val.is_empty());

	val = ini.get(_T("section2"), _T("no-such"));
	assert(val.is_empty());
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

