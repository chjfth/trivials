#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ps_TCHAR.h>
#include <msvc_extras.h>

#include "CHHI_DEBUG.h"

#undef NDEBUG // Enable assert for Release-build

#define CHHI_ALL_IMPL

#include <CHHI_vaDBG_is_vaDbgTs.h> // optional
#include <fsapi.h>
#include <makeTsdring.h>

#include <hashdict.h>
using namespace chjds;

#include "t_english_dict.h"

struct NumMap_st
{
	const TCHAR *name;
	int value;
};

NumMap_st gar_NumMap[] =
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


void test_hashdict_ints()
{
	hashdict<int> d1(_T("d1"));

	int i, *pi = nullptr;
	const int NUMs = ARRAYSIZE(gar_NumMap);

	d1.SetDbgParams(0);

	// prepare dict

	for(i=0; i<NUMs; i++)
	{
		NumMap_st &map = gar_NumMap[i];
		d1.set(map.name, map.value);
	}

	// test enumor
	const TCHAR * key = nullptr;

	_tprintf(_T("First round enumeration: (total %d)\n"), d1.keycount());
	hashdict<int>::enumor en1(d1);
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
		NumMap_st &map = gar_NumMap[i];
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
		NumMap_st &map = gar_NumMap[i];
		d1.del(map.name);
	}

	// test non-existence number keys
	for(i=0; i<DELs; i++)
	{
		NumMap_st &map = gar_NumMap[i];
		pi = d1.get(map.name);
		assert(!pi);
	}

	// Re-add those DELs
	for(i=0; i<DELs; i++)
	{
		NumMap_st &map = gar_NumMap[i];
		d1.set(map.name, map.value);
	}
	for(i=0; i<NUMs; i++)
	{
		NumMap_st &map = gar_NumMap[i];
		pi = d1.get(map.name);
		assert(pi);
		assert(*pi == map.value);
	}

	_tprintf(_T("Repeatedly add then del many times, to see trove grow then *compact*.\n"));

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

	// d1.CompactTrove(); // explicit compact, for debug purpose

	_tprintf(_T("Test mass del() triggering CompactTrove.\n"));

	for(i=0; i<15; i++)
	{
		NumMap_st &map = gar_NumMap[i];
		bool succ = d1.del(map.name);
		assert(succ);
	}
	
}

//////////////////////////////////////////////////////////////////////////

class CV 
{
public:
	int* m_pint;
public:
	CV() { 
		printf("CV() ctor @<%p>\n", this);
		m_pint = nullptr; 
	}
	CV(int n) { 
		m_pint = new int(n);
		printf("CV(%d) ctor @<%p>.\n", *m_pint, this);
	}
	virtual ~CV() {
		if(m_pint) {
			printf("CV(%d) dtor @<%p>.\n", *m_pint, this);
			delete m_pint;
		} else {
			printf("CV() dtor @<%p>.\n", this);
		}
	}

	CV(const CV& ins) { // copy-ctor
		printf("CV copy-ctor\n");
		if(ins.m_pint)
			m_pint = new int(*ins.m_pint);
		else
			m_pint = nullptr;
	}

	CV& operator=(const CV& ins) { // copy-assign
		printf("CV copy-assign\n");
		delete m_pint;
		if(ins.m_pint)
			m_pint = new int(*ins.m_pint);
		else
			m_pint = nullptr;
		return *this;
	}
#if 1
	CV(CV&& ins) {           // move-ctor
		if(ins.m_pint)
			printf("CV(%d) move-ctor @<%p> => @<%p>\n", *ins.m_pint, &ins, this);
		else
			printf("CV() move-ctor @<%p> => @<%p>\n", &ins, this);
		_steal_from_old(ins);
	}
	CV& operator=(CV&& old) { // move-assign
		if (this != &old) {
			if(old.m_pint)
				printf("CV(%d) move-assign\n", *old.m_pint);
			else
				printf("CV() move-assign\n");
			delete this->m_pint;
			_steal_from_old(old);
		}
		return *this;
	}
	void _steal_from_old(CV& old) {
		this->m_pint = old.m_pint;
		old.m_pint = nullptr;
	}
#endif
};

void test_hashdict_cxxobjs()
{
	// This verifies we can use complex C++ object as dict-value.

	_tprintf(_T("Do %s()\n"), _T(__FUNCTION__));

	hashdict<CV> d2(_T("d2"));
//	d2.SetDbgParams(0);

	int i, *pi=nullptr;
	CV *pcv = nullptr;

	const int CVs = 16;//ARRAYSIZE(gar_NumMap);
	for(i=0; i<CVs; i++)
	{
		NumMap_st &map = gar_NumMap[i];
		pcv = d2.set(map.name, CV(map.value));
		assert(*pcv->m_pint==map.value);
	}
#if 1
	d2.setdefault(_T("one"), 111);
	pcv = d2.get(_T("one"));
	assert(*pcv->m_pint==1); // should not change by setdefault()
	d2.set(_T("one"), 111);
	pcv = d2.get(_T("one"));
	assert(*pcv->m_pint==111);
	d2.set(_T("one"), 1); // restore 
#endif
	for(i=0; i<CVs; i++)
	{
		NumMap_st &map = gar_NumMap[i];
		pcv = d2.get(map.name);
		assert(*pcv->m_pint==map.value);
	}

}

void test_hashdict_copymove()
{
	printf("%s()\n", __FUNCTION__);
	bool succ = false;

	hashdict<int> x1(_T("x1"));
	int i, *pi = nullptr;
	const int NUMs = ARRAYSIZE(gar_NumMap);

	// fill x1
	for(i=0; i<NUMs; i++)
	{
		NumMap_st &map = gar_NumMap[i];
		x1.set(map.name, map.value);
	}

	// clone out x2 from x1
	hashdict<int> x2(x1);
	x2.set_dbgsig(_T("x2"));
	assert(x2.keycount()==NUMs);

	// create x3 and transfer x1 to x3

	hashdict<int> x3( std::move(x1) );
	x3.set_dbgsig(_T("x3"));
	assert(x3.keycount()==NUMs);
	assert(x1.keycount()==0);

	// delete "one" from x3
	succ = x3.del(_T("one"));
	assert(succ);
	pi = x3.get(_T("one"));
	assert(!pi);

	// move x2 to x3, this abandons x2
	x3 = std::move(x2);
	
	pi = x2.get(_T("one"));
	assert(!pi);
	
	pi = x3.get(_T("one"));
	assert(pi && *pi==1);
}

void Evernote_Demo() // Evclip: 20260413.m1
{
	hashdict<int> dict;			// init 8 slots

	dict.set(_T("one"), 1);		// slot #7
	dict.set(_T("two"), 2);		// slot #5
	dict.set(_T("three"), 3);	// slot (#7) #0
	dict.set(_T("four"), 4);	// slot #2
	
	dict.del(_T("one"));		// slot #7 is marked dummy
	int *pi = dict.get(_T("three"));
	assert(*pi==3);
}

void test_trivials()
{
	hashdict<int> dict(_T("AllDummy"));
	dict.SetDbgParams(0);

	int i, *pi=nullptr;
	for(i=0; i<8; i++)
	{
		NumMap_st &map = gar_NumMap[i];
		pi = dict.set(map.name, map.value);
		assert(*pi==map.value);
	}

	// Make all slots dummy
	for(i=0; i<8; i++)
	{
		NumMap_st &map = gar_NumMap[i];
		int oldval = -1;
		dict.del(map.name, oldval);
		assert(oldval = map.value);
	}

	pi = dict.set(_T("hundred"), 100);
	// -- Note: This set() will trigger slot-capacity increase, bcz dummy slots
	//    contribute to "full" state of slot storage.
	assert(*pi==100);
}


//////////////////////////////////////////////////////////////////////////

int _tmain(int argc, char *argv[])
{
	setlocale(LC_ALL, "");

#ifdef _MSC_VER
	// Take snapshot before operations
	_CrtMemState state1={}, state2={}, stateDiff={};
	_CrtMemCheckpoint(&state1);
#endif

//	test_trivials();

	Evernote_Demo();

	test_hashdict_ints(); 

	test_hashdict_cxxobjs();

	test_hashdict_copymove();

	printf("\n");

	bool succ = false;

	// Test big English dictionaries.

	int ar_resizepct[] = {66, 0};
	for(int i=0; i<ARRAYSIZE(ar_resizepct); i++)
	{
		int resizepct = ar_resizepct[i];
		printf("/////////////////////////////////////////////////////////////////////\n");
		printf("TestDict with resizepct = %d %s\n", resizepct,
			resizepct==0 ? "(Tight-loaded, no spare slots, slow)" : "");
		printf("/////////////////////////////////////////////////////////////////////\n");

		printf("\n");
		succ = t_english_dict(_T("..\\..\\_data\\english-words-22k.txt"), resizepct);
		//	t_english_dict(_T("..\\..\\_data\\english-words-22k.utf8.txt"));
		assert(succ);

		printf("\n");
		succ = t_english_dict(_T("..\\..\\_data\\english-words-4600k.txt"), resizepct);
		assert(succ);

		printf("\n");
	}


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

