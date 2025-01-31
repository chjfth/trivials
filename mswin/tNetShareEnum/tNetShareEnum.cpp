#include <windows.h>
#include <Lm.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

#include <EnsureClnup_mswin.h>
//#include <AutoBuf.h>
#include "../../__WinConst/itc/InterpretConst.h"
#include "../../__WinConst/mswin/LMAccess.itc.h"

#pragma comment(lib, "Netapi32.lib")

MakeCleanupPtrClass_winapi(Cec_NetApiBufferFree, NET_API_STATUS, NetApiBufferFree, PVOID)

using namespace itc;

const Enum2Val_st e2v_LmShareType[] =
{
	ITC_NAMEPAIR(STYPE_DISKTREE),     // 0
	ITC_NAMEPAIR(STYPE_PRINTQ),       // 1
	ITC_NAMEPAIR(STYPE_DEVICE),       // 2
	ITC_NAMEPAIR(STYPE_IPC),          // 3
};

const SingleBit2Val_st b2v_LmShareType[] =
{
	ITC_NAMEPAIR(STYPE_TEMPORARY),  // 0x40000000
	ITC_NAMEPAIR(STYPE_SPECIAL),    // 0x80000000
};

CInterpretConst STYPE_xxx(ITCF_HEX1B, e2v_LmShareType, ARRAYSIZE(e2v_LmShareType),
	b2v_LmShareType, ARRAYSIZE(b2v_LmShareType),
	nullptr, 0);



enum { Level1=1, Level2=2, Level3=3 };

void test_NetFileEnum()
{
	SHARE_INFO_2 *par_shinfo = NULL;
	DWORD entries_read = 0, entries_total = 0;
	DWORD_PTR resume_handle = 0;

	DWORD winerr = NetShareEnum(
		NULL, // servername
		Level2,
		(LPBYTE*)&par_shinfo,  // output buffer ptr
		MAX_PREFERRED_LENGTH, // prefmaxlen,
		&entries_read,
		&entries_total,
		&resume_handle); // resume-handle (no use due to MAX_PREFERRED_LENGTH)
	Cec_NetApiBufferFree cec_finfo = par_shinfo;

	if(winerr)
	{
		_tprintf(_T("NetShareEnum() fails with winerr=%d.\n"), winerr);
		return;
	}

	int i;
	for(i=0; i<(int)entries_total; i++)
	{
		SHARE_INFO_2 &shi = par_shinfo[i];
		_tprintf(_T("[#%d/%d]\n"), i+1, entries_total);
		_tprintf(_T("  .shi2_netname = %s\n"), shi.shi2_netname);
		_tprintf(_T("  .shi2_path    = %s\n"), shi.shi2_path);
		_tprintf(_T("  .shi2_type = 0x%X , %s\n"), shi.shi2_type, ITCSv(shi.shi2_type, STYPE_xxx));
		_tprintf(_T("  .shi2_remark = %s\n"), shi.shi2_remark);
		_tprintf(_T("  .shi2_max_uses = %d\n"), shi.shi2_max_uses);
		_tprintf(_T("  .shi2_current_uses = %d\n"), shi.shi2_current_uses);

		_tprintf(_T("  The following two are always 0/null on WinNT:\n"));
		_tprintf(_T("  .shi2_permissions = 0x%X , %s\n"), 
			shi.shi2_permissions, ITCSv(shi.shi2_permissions, ACCESS_xxx));
		_tprintf(_T("  .shi2_passwd = %s\n"), shi.shi2_passwd);

		_tprintf(_T("\n"));
	}
}


int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	_tprintf(_T("This code calls NetShareEnum() to list all shared-folders.\n"));

	test_NetFileEnum();

	return 0;
}
