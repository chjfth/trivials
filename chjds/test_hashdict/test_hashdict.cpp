#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <ps_TCHAR.h>

#ifdef __linux_
#include <linux/msvc_extras.h>
#endif

#undef NDEBUG // Enable assert for Release-build

//#define vaDbgTs_IMPL

#define CHHI_ALL_IMPL
//#define hashdict_DEBUG

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
		NumMap_st &map = gar_NumMap[i];
		bool succ = d1.del(map.name);
		assert(succ);
	}
	
}

//////////////////////////////////////////////////////////////////////////

//static int s_counter = 0;

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

//////////////////////////////////////////////////////////////////////////

#include <ctype.h>

template<typename TString>
class MakeUpper
{
public:
	MakeUpper(TString &s) : m_str(s){ }
	void DoUp()
	{
		for(int i=0; m_str[i]; i++)
			m_str[i] = toupper(m_str[i]);
	}

private:
	TString &m_str;
};

void test_uppper()
{
	Sdring sd1(_T("xyz"));
	MakeUpper<TCHAR*> up1(sd1);
	up1.DoUp();
	_tprintf(_T("Upper result: %s\n"), sd1.c_str());
}

int _tmain(int argc, char *argv[])
{
	setlocale(LC_ALL, "");
//	test_uppper();

#ifdef _MSC_VER
	// Take snapshot before operations
	_CrtMemState state1={}, state2={}, stateDiff={};
	_CrtMemCheckpoint(&state1);
#endif

	test_hashdict_ints();

	test_hashdict_cxxobjs();

	printf("\n");
	t_english_dict(_T("..\\..\\_data\\english-words-22k.txt"));
//	t_english_dict(_T("..\\..\\_data\\english-words-22k.utf8.txt"));

	printf("\n");
	t_english_dict(_T("..\\..\\_data\\english-words-4600k.txt"));

#ifdef _MSC_VER
	// Take snapshot after
	_CrtMemCheckpoint(&state2);

	// Compare snapshots
	if (_CrtMemDifference(&stateDiff, &state1, &state2)) 
	{
		_tprintf(_T("Memory leak detected!!! See debug channel for details.\n"));
		_CrtMemDumpStatistics(&stateDiff);
		_CrtMemDumpAllObjectsSince(&state1);
	}
#endif // _MSC_VER

	return 0;
}

