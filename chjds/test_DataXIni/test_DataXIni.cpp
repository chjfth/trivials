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


bool test0()
{
	const TCHAR *inisrc = _T("DigClock2.ini");
	const TCHAR *inidst = _T("output0.ini");

	SimpleIni iniobj;
	auto err1 = iniobj.load(inisrc);
	assert(!err1);

	// Define DataXString objects for INI-concerned data.
	DataXString<ClockMode_et> clockmode(_T("CM_WallTime"));
	DataXString<bool> isOnTop(false); // false is default vale

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

	return (err1 || err2) ? false : true;
}

int _tmain(int argc, TCHAR *argv[])
{
	bool isok = false;
//	isok = file_copy(_T("DigClock2.ini"), _T("test0.ini"), true);
//	assert(isok);

	isok = test0();
	_tprintf(_T("test0() %s.\n"), isok?_T("success"):_T("fail"));

	return 0;
}
