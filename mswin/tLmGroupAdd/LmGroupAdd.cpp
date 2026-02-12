#include <windows.h>
#include <LM.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

#include <EnsureClnup_mswin.h>
#include <mswin/LMErr.itc.h>
using namespace itc;

void test_NetLocalGroupAdd(const TCHAR *servername, const TCHAR *username)
{

	TCHAR comment[100] = {};
	_sntprintf_s(comment, _TRUNCATE, _T("%s comment"), username);

	LOCALGROUP_INFO_1 gi = {};
	gi.lgrpi1_name = (TCHAR*)username;
	gi.lgrpi1_comment = comment;

	DWORD ErrParm = 0;
	NET_API_STATUS neterr = NetLocalGroupAdd(servername, 1, (PBYTE)&gi, &ErrParm);
	if(neterr)
	{
		_tprintf(_T("NetLocalGroupAdd() exec fail, neterr=%s.\n"), ITCSv(neterr, NERR_xxx));
		if(neterr==ERROR_INVALID_PARAMETER) // 87
			_tprintf(_T("ErrParam = %d.\n"), ErrParm);
	}
	else
	{
		_tprintf(_T("NetLocalGroupAdd() exec OK.\n"));
	}
}

struct S1
{
	char data[4];
};

void testS1(S1 *ps1)
{

}

void dotest()
{
	//testS1(&{1});
}

int _tmain(int argc, TCHAR* argv[])
{
	dotest();

	if(argc==1)
	{
		printf("tLmGroupAdd v1.1\n");
		printf("Usage:\n");
		printf("    tLmGroupAdd <groupname>\n");
		printf("    tLmGroupAdd <groupname> [servername]\n");
		exit(1);
	}

	setlocale(LC_ALL, "");
	
	const TCHAR * username = argv[1];
	const TCHAR * servername = NULL;
	if(argc>2)
		servername = argv[2];

	test_NetLocalGroupAdd(servername, username);

	return 0;
}

