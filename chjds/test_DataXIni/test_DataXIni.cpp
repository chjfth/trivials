#include <CHHI_DEBUG.h>
#include <assert.h>
#include <stdio.h>
#include <ps_TCHAR.h>
#include <msvc_extras.h>
#include <fsapi.h>
	using namespace fsapi;
#include <SimpleIni.h>
#include <DataXIni.h>


enum ClockMode_et { CM_WallTime = 0, CM_Countdown = 1 };

template<typename FORMAT>
struct DataXTraits<ClockMode_et, FORMAT>
{
	static ClockMode_et FromString(const TCHAR* s)
	{
		if (shp_stricmp(s, _T("CM_WallTime")) == 0)
			return CM_WallTime;
		else if (shp_stricmp(s, _T("CM_Countdown")) == 0)
			return CM_Countdown;
		else
			return CM_WallTime;
	}

	static Sdring ToString(ClockMode_et val)
	{
		if (val == CM_Countdown)
			return _T("CM_Countdown");
		else
			return _T("CM_WallTime");
	}
};


bool do_test1()
{
	const TCHAR *inisrc = _T("DigClock2.ini"); // read-only, don't change its content
	const TCHAR *inidst = _T("output0.ini");

	file_delete(inidst);
	bool is_old_output = file_exists(inidst);
	assert(!is_old_output);

	SimpleIni iniobj;
	auto err1 = iniobj.load(inisrc);
	assert(!err1);

	// Define DataXString objects for INI-concerned data.
	DataXString<ClockMode_et> clockmode(_T("CM_WallTime"));
	DataXString<bool> isOnTop(_T("false")); // false is default value

	// Set new value
	clockmode = CM_Countdown;
	isOnTop = true;

	// Sync new value to iniobj, operation in RAM
	const TCHAR *secname = _T("cfg"); // INI section name
	iniobj.set(secname, _T("ClockMode"), clockmode.GetValueAsString());
	iniobj.set(secname, _T("AlwaysOnTop"), isOnTop.GetValueAsString());

	// Finally, write INI content to diskfile.
	auto err2 = iniobj.save(inidst);
	assert(!err2);

	if(err1 || err2) 
		return false;

	//
	// Test DataXIni with DataXString_AutoSaveIni
	//

	DataXIni xini;
	xini.LoadIni(&inidst, 1);

	DataXString_AutoSaveIni<bool> isShowTitle(xini, secname, _T("IsShowTitle"), _T("false"));

	// Inside `isShowTitle = true;`, INI saving is done automatically
	isShowTitle = true;

	// Verify above result
	SimpleIni iniobjv;
	auto err3 = iniobjv.load(inidst);
	assert(!err3);
	Sdring sdShowTitle = iniobjv.get(secname, _T("IsShowTitle"));
	assert( Sdring::str_match(sdShowTitle, _T("true")) );

	return true;
}

int _tmain(int argc, TCHAR *argv[])
{
	MSVCRT_MemCheckStart(foo);

	bool isok = false;
//	isok = file_copy(_T("DigClock2.ini"), _T("test0.ini"), true);
//	assert(isok);

	isok = do_test1();

	_tprintf(_T("do_test1() %s.\n"), isok?_T("success"):_T("fail"));

	bool isleak = MSVCRT_MemCheckEnd_IsLeak(foo);
	if (isleak) {
		printf("Memleak in my C++ code!!!\n");
		return 4;
	}
	else {
		printf("(No memleak.)\n");
		return 0;
	}

	return 0;
}
