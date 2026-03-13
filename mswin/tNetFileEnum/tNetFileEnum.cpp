#include <windows.h>
#include <Lm.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

#define CHHI_ALL_IMPL

#include <EnsureClnup_mswin.h>
//#include <JAutoBuf.h>

MakeCleanupPtrClass_winapi(Cec_NetApiBufferFree, NET_API_STATUS, NetApiBufferFree, PVOID)


enum { Level3=3 };

void test_NetFileEnum()
{
	FILE_INFO_3 *par_finfo = NULL;
	DWORD entries_read = 0, entries_total = 0;
	DWORD_PTR resume_handle = 0;

	DWORD winerr = NetFileEnum(
		NULL, // servername
		NULL, // query path prefix
		NULL, // username? 
		Level3,
		(LPBYTE*)&par_finfo,  // output buffer ptr
		MAX_PREFERRED_LENGTH, // prefmaxlen,
		&entries_read,
		&entries_total,
		&resume_handle); // resume-handle
	Cec_NetApiBufferFree cec_finfo = par_finfo;


	if(winerr)
	{
		_tprintf(_T("NetFileEnum() fails with winerr=%d.\n"), winerr);
		return;
	}
}


int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	_tprintf(_T("This code calls NetFileEnum() to list all shared-folders. (Not complete yet.)\n"));

	test_NetFileEnum();

	return 0;
}

