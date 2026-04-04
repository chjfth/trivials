#include <stdio.h>
#include <stdlib.h>
#include <ps_TCHAR.h>

#ifdef __linux_
#include <linux/msvc_extras.h>
#endif

#undef NDEBUG // Enable assert for Release-build

#define vaDbgTs_IMPL
#include <CHHI_vaDBG_is_vaDbgTs.h> // optional

#define CHHI_ALL_IMPL
#define hashdict_DEBUG

#include <hashdict.h>
using namespace chjds;

struct NumMap_st
{
	const TCHAR *name;
	int value;
};

NumMap_st gar_nummap[] =
{
	{ _T("one"), 1, },
	{ _T("two"), 2, },
	{ _T("three"), 3, },
	{ _T("four"), 4, },
	{ _T("five"), 5, },
	{ _T("six"), 6, },
	{ _T("seven"), 7, },
	{ _T("eight"), 8, },
	{ _T("nine"), 9, },
	{ _T("ten"), 10, },
	{ _T("eleven"), 11, },
	{ _T("twelve"), 12, },
	{ _T("thirteen"), 13, },
	{ _T("fourteen"), 14, },
	{ _T("fifteen"), 15, },
	{ _T("sixteen"), 16, },
	{ _T("seventeen"), 17, },
	{ _T("eighteen"), 18, },
	{ _T("nineteen"), 19, },
	{ _T("twenty"), 20, },
	{ _T("twenty-one"), 21, },
};

NumMap_st gar_MonthMap[] = 
{
	{ _T("January"), -1, },
	{ _T("February"), -2, },
	{ _T("March"), -3, },
	{ _T("April"), -4, },
	{ _T("May"), -5, },
	{ _T("June"), -6, },
	{ _T("July"), -7, },
	{ _T("August"), -8, },
	{ _T("September"), -9, },
	{ _T("October"), -10, },
	{ _T("November"), -11, },
	{ _T("December"), -12, },
};


void test_hashdict()
{
	hashdict<int> d1; //(_T("d1"));

	int i, *pi = nullptr;
	const int NUMs = ARRAYSIZE(gar_nummap);

	d1.SetDbgParams(0);

	// prepare dict

	for(i=0; i<NUMs; i++)
	{
		NumMap_st &map = gar_nummap[i];
		d1.set(map.name, map.value);
	}

	// test enumer
	const TCHAR * key = nullptr;

	_tprintf(_T("First round enumeration: (total %d)\n"), d1.keycount());
	hashdict<int>::enumer en1(d1);
	for(i=0; ;i++)
	{
		int *pvalue = nullptr;
		key = en1.next(&pvalue);

		if(i==0 || i==1)
		{
			pi = d1.set(_T("invader"), 444);
			assert(!pi); 
		}

		if(key)
			_tprintf(_T("key#%d '%s' => %d\n"), i, key, *pvalue);
		else
			break;
	}

	key = en1.next(); // deliberate 
	assert(key==nullptr);

	en1.reset();

	// test existing keys 
	//
	for(i=0; i<NUMs; i++)
	{
		NumMap_st &map = gar_nummap[i];
		pi = d1.get(map.name);
		assert(pi);
		assert(*pi == map.value);
	}

	// test non-existence month keys
	//
	for(i=0; i<ARRAYSIZE(gar_MonthMap); i++)
	{
		NumMap_st &map = gar_MonthMap[i];
		pi = d1.get(map.name);
		assert(!pi);
	}

	const int zero = -100;
	pi = d1.setdefault(_T("zero"), zero);
	assert(pi && *pi==zero);

	pi = d1.setdefault(_T("one"), 111);
	assert(pi && *pi==1);

	// Test [del + re-add]
	for(i=0; i<3; i++)
	{
		bool succ = d1.del(_T("zero"));
		assert(succ);
		pi = d1.set(_T("zero"), zero);
		assert(pi && *pi==zero);
	}

	// Test del batch

	const int DELs = 5;
	for(i=0; i<DELs; i++)
	{
		NumMap_st &map = gar_nummap[i];
		d1.del(map.name);
	}

	// test non-existence number keys
	for(i=0; i<DELs; i++)
	{
		NumMap_st &map = gar_nummap[i];
		pi = d1.get(map.name);
		assert(!pi);
	}

	// Re-add those DELs
	for(i=0; i<DELs; i++)
	{
		NumMap_st &map = gar_nummap[i];
		d1.set(map.name, map.value);
	}
	for(i=0; i<NUMs; i++)
	{
		NumMap_st &map = gar_nummap[i];
		pi = d1.get(map.name);
		assert(pi);
		assert(*pi == map.value);
	}

	vaDbgTs(_T("Repeatedly add then del many times, to see trove grow then *compact*."));

	const TCHAR *jankey = gar_MonthMap[0].name;
	int janval = gar_MonthMap[0].value;
	const TCHAR *febkey = gar_MonthMap[1].name;
	int febval = gar_MonthMap[1].value;

	for(i=0; i<10; i++)
	{
		d1.set(jankey, janval);
		d1.set(febkey, febval);

		d1.del(jankey);
		d1.del(febkey);
	}

	d1.set(_T("FinalKey"), 0);

	_tprintf(_T("Second round enumeration: (total %d)\n"), d1.keycount());
	en1.reset();
	for(i=0; ;i++)
	{
		int *pvalue = nullptr;
		key = en1.next(&pvalue);

		if(key)
			_tprintf(_T("key#%d '%s' => %d\n"), i, key, *pvalue);
		else
			break;
	}
	assert(i==d1.keycount());


	vaDbgTs(_T("Test mass del() triggering CompactTrove."));

	// d1.CompactTrove(); // explict compact, for debug purpose
	for(i=0; i<15; i++)
	{
		NumMap_st &map = gar_nummap[i];
		bool succ = d1.del(map.name);
		assert(succ);
	}
	
}

#ifdef _MSC_VER
int _tmain(int argc, TCHAR* argv[])
#else
int main(int argc, char *argv[])
#endif
{
#ifdef _MSC_VER
	// Take snapshot before operations
	_CrtMemState state1={}, state2={}, stateDiff={};
	_CrtMemCheckpoint(&state1);
#endif

	test_hashdict();

#ifdef _MSC_VER
	// Take snapshot after
	_CrtMemCheckpoint(&state2);

	// Compare snapshots
	if (_CrtMemDifference(&stateDiff, &state1, &state2)) 
	{
		_CrtMemDumpStatistics(&stateDiff);
		_CrtMemDumpAllObjectsSince(&state1);
	}
#endif // _MSC_VER

	return 0;
}

