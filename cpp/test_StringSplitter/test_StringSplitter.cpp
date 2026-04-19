#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <string>
#include <iostream>
using namespace std;

#include <msvc_extras.h>

#include <StringHelper.h>

bool SplitBySemicolon(int c)
{
	return c == ';' ? true : false;
}

inline bool TrimSpace(int c)
{
	return c == ' ' ? true : false;
}

template<typename TSplitter>
void printf_subs(TSplitter& sp, const TCHAR *instr)
{
	for (int i = 0;; i++)
	{
		int len = 0;
		int pos = sp.next(&len);
		if (pos == -1)
			break;

		_tprintf(_T("[#%d](%2d+%2d) \"%.*s\"\n"), i,
			pos, len, // (%2d+%2d)
			len, instr + pos);
	}
}

void do_test()
{
	const TCHAR *instr = nullptr;

	instr = _T(";AB;xyz; ; 1234;");
	_tprintf(_T("\n==========================\n"));
	_tprintf(_T("\"%s\"  (SplitBySemicolon, TrimSpace)\n"), instr);
	//
	StringSplitter<const TCHAR*, SplitBySemicolon, TrimSpace, false> sp(instr);
	printf_subs(sp, instr);


	_tprintf(_T("\n==========================\n"));
	_tprintf(_T("\"%s\"  (SplitBySemicolon, TrimSpace, want_null_split)\n"), instr);
	//
	StringSplitter<const TCHAR*, SplitBySemicolon, TrimSpace, true> sp_want_null(instr);
	printf_subs(sp_want_null, instr);


	instr = _T("");
	_tprintf(_T("\n==========================\n"));
	_tprintf(_T("\"%s\"  (SplitBySemicolon, null-string, want_null_split)\n"), instr);
	//
	StringSplitter<const TCHAR*, SplitBySemicolon, TrimSpace, true> sp_want_null1(instr);
	printf_subs(sp_want_null1, instr);


	instr = _T(";");
	_tprintf(_T("\n==========================\n"));
	_tprintf(_T("\"%s\"  (SplitBySemicolon, single split char, want_null_split)\n"), instr);
	StringSplitter<const TCHAR*, SplitBySemicolon, TrimSpace, true> sp_want_null2(instr);
	printf_subs(sp_want_null2, instr);


	instr = _T(";X");
	_tprintf(_T("\n==========================\n"));
	_tprintf(_T("\"%s\"  (SplitBySemicolon, want_null_split)\n"), instr);
	StringSplitter<decltype(instr), SplitBySemicolon, TrimSpace, true> sp_want_null3(instr);
	printf_subs(sp_want_null3, instr);

	// To make it run with Linux-gcc, we do not use wstring for ss.
	string ss = ";AB;xyz; ; 1234;";
	printf("\n========= Use with C++ STL string =========\n");
	printf("\"%s\"  *SplitBySemicolon, TrimSpace)\n", ss.c_str());

	StringSplitter<string, SplitBySemicolon, TrimSpace> sp_wstring(ss, 0, ss.length());
	for (int i = 0;; i++)
	{
		int len = 0;
		int pos = sp_wstring.next(&len);
		if (pos == -1)
			break;

		string sub = ss.substr(pos, len);
		printf("[#%d](%2d+%2d) \"%s\"\n", i,
			pos, len, // (%2d+%2d)
			sub.c_str());
	}

}

int _tmain(int argc, TCHAR* argv[])
{
	do_test();
	return 0;
}

/* [2026-04-19] Correct output:

==========================
";AB;xyz; ; 1234;"  (SplitBySemicolon, TrimSpace)
[#0]( 1+ 2) "AB"
[#1]( 4+ 3) "xyz"
[#2](11+ 4) "1234"

==========================
";AB;xyz; ; 1234;"  (SplitBySemicolon, TrimSpace, want_null_split)
[#0]( 0+ 0) ""
[#1]( 1+ 2) "AB"
[#2]( 4+ 3) "xyz"
[#3]( 9+ 0) ""
[#4](11+ 4) "1234"
[#5](16+ 0) ""

==========================
""  (SplitBySemicolon, null-string, want_null_split)
[#0]( 0+ 0) ""

==========================
";"  (SplitBySemicolon, single split char, want_null_split)
[#0]( 0+ 0) ""
[#1]( 1+ 0) ""

==========================
";X"  (SplitBySemicolon, want_null_split)
[#0]( 0+ 0) ""
[#1]( 1+ 1) "X"

========= Use with C++ STL string =========
";AB;xyz; ; 1234;"  *SplitBySemicolon, TrimSpace)
[#0]( 1+ 2) "AB"
[#1]( 4+ 3) "xyz"
[#2](11+ 4) "1234"

*/

