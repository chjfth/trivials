#undef NDEBUG
#include <assert.h>
#include <utility>

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

//#include <commdefs.h>

#define CHHI_ALL_IMPL
#include <ospath.h>
using namespace ospath;

#define ASSERT_MATCH(s1, s2) assert(Sdring::str_match(s1, s2))

inline Sdring _Paths_join2(const TCHAR *Path1, const TCHAR *Path2, TCHAR sepchar)
{
	// User input Path1 & Path2 should always contain forward slashes(/).
	// If sepchar=='\\', all / in Path1 & Path2 will be replaced with \ then to
	// call prestine paths_join2().

	int i;
	Sdring s1(Path1);
	Sdring s2(Path2);

	if(sepchar=='\\')
	{
		int len1 = (int)_tcslen(Path1);
		for(i=0; i<len1; i++)
		{
			if(s1[i]=='/')
				s1[i] = '\\';
		}

		int len2 = (int)_tcslen(Path2);
		for(i=0; i<len2; i++)
		{
			if(s2[i]=='/')
				s2[i] = '\\';
		}
	}

	Sdring sret = paths_join2(s1, s2, sepchar);

	if(sepchar=='\\')
	{
		int lenret = (int)_tcslen(sret);

		// NOTE: Not considering '\' in MBCS here.
		for(i=0; i<lenret; i++)
		{
			if(sret[i]=='\\')
				sret[i] = '/';
		}
	}

	return sret;
}

#define CHECK_JOIN2(in1, in2, answer) \
	p1 = _T(in1); \
	p2 = _T(in2); \
	sret = _Paths_join2(p1, p2, sepchar); \
	assert(Sdring::str_match(sret, _T(answer))); \


void test_path_join(TCHAR sepchar)
{
	const TCHAR *p1=0, *p2=0, *p3=0;
	Sdring sret;

	// Join a fullpath and a relapath.

	CHECK_JOIN2("d:/abc/def", "123",
		"d:/abc/def/123");

	CHECK_JOIN2("d:/abc/def", "../123",
		"d:/abc/123");

	CHECK_JOIN2("d:/abc/def", "../../123", 
		"d:/123");

	CHECK_JOIN2("d:/abc/def", 
		"../../../123", // Verbose ".." levels are silently discarded.
		"d:/123");

	CHECK_JOIN2("d:/abc/def", "/123", 
		"d:/123");

	CHECK_JOIN2("d:/abc/def", "./123/../456", 
		"d:/abc/def/456");

	// We allow that first path is a relative path.

	CHECK_JOIN2("abc/def", "123/456",
		"abc/def/123/456");

	CHECK_JOIN2("abc/def", "d:/123/456",
		"d:/123/456");

	// Extra "." nodes will be removed, midway or at tail.

	CHECK_JOIN2("d:/abc/./def", "./123/.././456/.", 
		"d:/abc/def/456");

	CHECK_JOIN2(".", "123", 
		"123");

	// If path1=="", the effect is to purify path2.
	// = path_normalize()
	CHECK_JOIN2("", "123/./../456", 
		"456");

	CHECK_JOIN2("", "d:/123/./../456", 
		"d:/456");

	// Allow begin with relative paths

	CHECK_JOIN2("", "../../123",
		"../../123");

	CHECK_JOIN2("../abc", "",     "../abc");

	CHECK_JOIN2("../../abc", "../123",
		"../../123");

	// Verbose slashes are compacted.

	CHECK_JOIN2("d:/abc//.//def/", "123///./../456//", 
		"d:/abc/def/456");

	// path2 is empty, we'll path1 verbatim
	// Python 3.9 will result in a trailing slash, which I frown upon.

	CHECK_JOIN2("d:/abc", "", 
		"d:/abc");

	//
	// Unix style root dir, no drive-letter.
	//

	CHECK_JOIN2("/abc/def", "123/./../456", 
		"/abc/def/456");

	//
	// Mix slash styles
	//

	p1 = _T("d:\\abc\\def");
	p2 = _T("../123");
	sret = paths_join2(p1, p2, sepchar);
	if(sepchar=='\\')
		ASSERT_MATCH(sret, _T("d:\\abc\\123"));
	else
		ASSERT_MATCH(sret, _T("d:/abc/123"));

}


Sdring _Fullpath_to_rela(const TCHAR *basedir, const TCHAR *tofullpath,  
	TCHAR sepchar, FTR_feedback_st *pfeedback, CaseSensitive_et cas)
{
	// User input basedir & tofullpath should always contain forward slashes(/).
	// If sepchar=='\\', all / in Path1 & Path2 will be replaced with \ then to
	// call prestine fullpath_to_rela().

	int i;
	Sdring sbase(basedir);
	Sdring sfull(tofullpath);

	if(sepchar=='\\')
	{
		int baselen = (int)_tcslen(sbase);
		for(i=0; i<baselen; i++)
		{
			if(sbase[i]=='/')
				sbase[i] = '\\';
		}

		int fulllen = (int)_tcslen(sfull);
		for(i=0; i<fulllen; i++)
		{
			if(sfull[i]=='/')
				sfull[i] = '\\';
		}
	}

	Sdring sret = fullpath_to_rela(sbase, sfull, sepchar, pfeedback, cas);

	if(sret.not_empty() && sepchar=='\\')
	{
		int retlen = (int)_tcslen(sret);

		// NOTE: Not considering '\' in MBCS here.
		for(i=0; i<retlen; i++)
		{
			if(sret[i]=='\\')
				sret[i] = '/';
		}
	}

	return sret;
}



#define CHECK_FTR_CaseParam(Full, Base, cas, Answer, nParents, isReachRoot) \
	pfull = _T(Full); \
	pbase = _T(Base); \
	sret = _Fullpath_to_rela(pfull, pbase, sepchar, &feedback, cas); \
	assert(Sdring::str_match(sret, _T(Answer))); \
	assert(feedback.nparents==nParents); \
	assert(feedback.is_reach_root==isReachRoot); \

#define CHECK_FTR(Full, Base, Answer, nParents, isReachRoot) \
	CHECK_FTR_CaseParam(Full, Base, CaseSense_yes, Answer, nParents, isReachRoot)

#define CHECK_FTR_IgnoreCase(Full, Base, Answer, nParents, isReachRoot) \
	CHECK_FTR_CaseParam(Full, Base, CaseSense_no, Answer, nParents, isReachRoot)

void test_fullpath_to_rela(TCHAR sepchar)
{
	const TCHAR *pbase=0, *pfull=0;
	Sdring sret;

	FTR_feedback_st feedback = {};

	// Most common case: Fullpath totally inside Basedir.

	CHECK_FTR("d:/abc", "d:/abc/def",
		"def", 0, false);

	CHECK_FTR("d:/abc", "d:/abc/def/123.txt", 
		"def/123.txt", 0, false);

	// Basedir must go up one or more parents first, only then can reach Fullpath.

	CHECK_FTR("d:/abc/def", "d:/abc/123.txt", 
		"../123.txt", 1, false);

	CHECK_FTR("d:/abc/def", "d:/123.txt", 
		"../../123.txt", 2, true);

	CHECK_FTR("/abc/def", "/123.txt",  // Unix root-dir style
		"../../123.txt", 2, true);

	CHECK_FTR("d:/abc/def", "d:/", 
		"../..", 2, true);

	// Basedir and Fullpath are from different windows drive-letter

	CHECK_FTR("c:/abc/def", "d:/123/456",
		"d:/123/456", 0, false);

	CHECK_FTR("/abc/def", "d:/123/456",
		"d:/123/456", 0, false);

	CHECK_FTR("c:/abc/def", "/123/456",
		"/123/456", 0, false);

	// Equal Basedir and Fullpath, a dot is returned.
	CHECK_FTR("c:/abc/def", "c:/abc/def",
		".", 0, false);
	CHECK_FTR("c:/abc/def/", "c:/abc/def",
		".", 0, false);
	CHECK_FTR("c:/abc/def", "c:/abc/def/",
		".", 0, false);


	// Wacky input: Input dir NOT in full-path form.

	CHECK_FTR("c:/abc/def", "123/456",
		"", 0, false);
	CHECK_FTR("abc/def", "d:/123/456",
		"", 0, false);

	// Test for redundant slashes.

	CHECK_FTR("d://abc/./def", "d:/xxx/.././123.txt", 
		"../../123.txt", 2, true);

	// Case-insensitive test-cases

	CHECK_FTR_IgnoreCase("D:/ABC", "d:/abc/def/123.txt", 
		"def/123.txt", 0, false);

	CHECK_FTR_IgnoreCase("d:/Abc/Def", "D:/abc/123.txt", 
		"../123.txt", 1, false);
}


int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	_tprintf(_T("Hello, test_ospath!\n"));

	test_path_join('/');
	test_path_join('\\');

	test_fullpath_to_rela('/');
	test_fullpath_to_rela('\\');

	_tprintf(_T("Success test_ospath.\n"));
	return 0;
}

