#include "CHHI_DEBUG.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ps_TCHAR.h>
#include <commdefs.h>
#include <locale.h>
#include <utility> // On MSVC, this brings in _CrtMemState()
#include <msvc_extras.h>
#include <vaDbgTs.h>
#include <snTprintf.h>
#include <sdring.h>
#include <makeTsdring.h>
#include <chj_mishmash.h>
#include <fsapi.h>
#include <ospath.h>

#include <SimpleIni.h>
#include <SimpleIniEx.h>

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
	SimpleIni::ReCode_et err = ini.load(inifilename);
	assert(!err);

	int i, DiffAt=-1;
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
'basedir' = 'd:/testdir'\n\
\n\
Section#2 [section1]\n\
'key1' = '11'\n\
'key2' = '12'\n\
'keym' = 'val first line\n\
val second line'\n\
'greet words' = 'Hello, my friend!'\n\
\n\
Section#3 [section2]\n\
'key1' = '21'\n\
'key2' = '22'\n\
'keym' = '\n\
val line one\n\
val line two\n\
val line three'\n\
'emptykey' = ''\n\
'endkey' = 'endval'\n\
\n\
Section#4 [unicodes]\n\
'key1' = 'ABC\x7535'\n\
\n"; //Note: \x7535 is a Unicode code-point(GBK dian).
	assert( sdring<wchar_t>::str_match(bigstrW, answerW, &DiffAt) );

	assert( ini.has_key(_T("section1"), _T("key2")) );
	assert(!ini.has_key(_T("section1"), _T("keyZ")) );

	Sdring val;
	bool haskey = false;
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
	haskey = ini.has_key(_T("section2"), _T("no-such"));
	assert(!haskey);

	val = ini.get(_T("section2"), _T("emptykey"));
	assert(val.is_empty());
	haskey = ini.has_key(_T("section2"), _T("emptykey"));
	assert(haskey);


	//
	// Set some new key-value pairs, then verify
	//

	const TCHAR *key2val = _T("New value \nwith second line");
	const TCHAR *endkeyval = _T("new endval");

	ini.set(_T("section2"), _T("key2"), key2val);
	Sdring newval1 = ini.get(_T("section2"), _T("key2"));

	ini.set(_T("section2"), _T("endkey"), endkeyval);
	Sdring newval2 = ini.get(_T("section2"), _T("endkey"));

	assert( Sdring::str_match(newval1, key2val, &DiffAt) );
	assert( Sdring::str_match(newval2, endkeyval, &DiffAt) );

	const TCHAR *in_newsec = _T("Fruits");
	const TCHAR *in_newkey1 = _T("Apple");
	const TCHAR *in_newkey2 = _T("Pear");
	ini.set(in_newsec, in_newkey1, _T("1"));
	ini.set(in_newsec, in_newkey2, _T("2"));

	Sdring retval1 = ini.get(in_newsec, in_newkey1);
	assert( Sdring::str_match(retval1, _T("1"), &DiffAt) );

	// Save whole INI content.

	Sdring initext_gen1 = ini.save_ini_string(_T("\n"));

	const TCHAR *saved_inifile = _T("sample1.save.ini");

	err = ini.save(saved_inifile, _T("\r\n")); // Let disk file use \r\n, even on Linux.
	assert(!err);

	// Read back the saved ini to verify it's content preserves.

	SimpleIni ini2 ;
	err = ini2.load(saved_inifile);
	assert(!err);

	Sdring initext_gen2 = ini2.save_ini_string(_T("\n"));

	assert( Sdring::str_match(initext_gen1, initext_gen2, &DiffAt) );

	// Final verify with stock answer file.
	
	const TCHAR *std_answer_file = _T("sample1.mod.ini");
	assert( binfile_is_match(saved_inifile, std_answer_file, &DiffAt) );

	_tprintf(_T("\n"));
}

void test_iniEx()
{
	_tprintf(_T("Test SimpleIniEx ...\n"));

	const TCHAR * const ar_inifiles[] =
	{
		_T("tier1.ini"), _T("tier2.ini"), _T("tier3.ini")
	};

	const TCHAR *ini_need_exists = ar_inifiles[1];
	const TCHAR *ini_output_file = ar_inifiles[2];

	SimpleIniEx ini;
	bool succ = ini.load_cascade(ar_inifiles, ARRAY_SIZE(ar_inifiles));
	if(!succ) {
		_tprintf(_T("[ERROR]Test input-file %s SHOULD exist for the test to run.\n"), ini_need_exists);
	}
	assert(succ);

	// Here, we mark tier2.ini readonly, so that later save_cascade() will write to tier3.ini.

	succ = ospath::file_mark_readonly(ini_need_exists);
	if(!succ) {
		_tprintf(_T("[ERROR]Test input-file %s marking readonly -- fail!\n"), ini_need_exists);
	}
	assert(succ);

	// Write current-time as key-value.
	TCHAR sznowtime[40];
	va_now_ymdhms(sznowtime, ARRAY_SIZE(sznowtime));

	SimpleIni::ReCode_et err = ini.set(_T("foosec"), _T("nowtime"), sznowtime);
	assert(!err);
	
	Sdring sout_inifile;
	succ = ini.save_cascade(&sout_inifile);
	assert(succ);

	// Verify the written content.

	SimpleIni iniout;
	err = iniout.load(ini_output_file);
	assert(!err);

	Sdring rs_nowtime = iniout.get(_T("foosec"), _T("nowtime"));
	assert( Sdring::str_match(sznowtime, rs_nowtime) );
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

	test_iniEx();

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

	printf("Success.\n");
	return 0;
}

