#include <windows.h>
#include <Lm.h>
#include <aclapi.h>
#include <Sddl.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

#include <EnsureClnup_mswin.h>
//#include <AutoBuf.h>
#include "../../__WinConst/itc/InterpretConst.h"
#include "../../__WinConst/mswin/LMAccess.itc.h"
#include "../../__WinConst/mswin/winnt.itc.h"

#pragma comment(lib, "Netapi32.lib")
#pragma comment(lib, "Advapi32.lib")

inline void _LocalFree_pstr(TCHAR *pstr){ LocalFree(pstr); } // for ConvertSidToStringSid
MakeCleanupPtrClass(Cec_LocalFree_pstr, void, _LocalFree_pstr, TCHAR*)
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



void print_AccessMask(DWORD accessMask) 
{
	const TCHAR *summary = _T("???");
	if(accessMask & WRITE_DAC)
	{	
		// share-folder Full-Control: 0x1F01FF
		summary = _T("Full-control");
	}
	else if(accessMask & DELETE)
	{
		// share-folder Change: 0x1301BF
		summary = _T("Can-change");
	}
	else if(accessMask & READ_CONTROL)
	{
		// share-folder Read: 0x1200A9
		summary = _T("Can-read");
	}

	_tprintf(_T("      <%s> 0x%06X %s\n"), 
		summary, accessMask,
		ITCSv(accessMask, DirectoryRights));
}

void print_SidAccountName(SID &sid)
{
	PSID pSid = &sid;
	TCHAR accountName[256]={}, domainName[256]={};
	DWORD accountSize = sizeof(accountName) / sizeof(TCHAR);
	DWORD domainSize = sizeof(domainName) / sizeof(TCHAR);
	SID_NAME_USE sidType = SidTypeInvalid; // neg-init

	BOOL succ = LookupAccountSid(NULL, pSid, accountName, &accountSize, domainName, &domainSize, &sidType);
	if(!succ)
	{
		_tprintf(_T("      Panic! LookupAccountSid() error, winerr=%d\n"), GetLastError());
		return;
	}

	if(domainName[0])
		_tprintf(_T("      (%s) %s\\%s\n"), ITCS(sidType, SidTypeXXX), domainName, accountName);
	else
		_tprintf(_T("      (%s) %s\n"), ITCS(sidType, SidTypeXXX), accountName);
}

void print_AceType(BYTE acetype)
{
	const TCHAR *pszAcetype = _T("AceType unknown!");
	if(acetype==ACCESS_ALLOWED_ACE_TYPE)
		pszAcetype = _T("ACCESS_ALLOWED_ACE_TYPE");
	else if(acetype==ACCESS_DENIED_ACE_TYPE)
		pszAcetype = _T("ACCESS_DENIED_ACE_TYPE");

	_tprintf(_T("      AceType %d: %s\n"), acetype, pszAcetype);
}

void print_SharePermissions(LPCWSTR shareName) 
{
	SHARE_INFO_502* pInfo = NULL;
	DWORD winerr = NetShareGetInfo(NULL, (LPWSTR)shareName, 502, (LPBYTE*)&pInfo);
	Cec_NetApiBufferFree cec_shinfo = pInfo;
	if (winerr) {
		printf("NetShareGetInfo failed. winerr=%d\n", winerr);
		return;
	}

	// Get security descriptor from SHARE_INFO_502
	PSECURITY_DESCRIPTOR pSD = pInfo->shi502_security_descriptor;
	if (!pSD) {
		// This occurs for IPC$, C$, D$ etc, no problem, just ignore it.
		// printf("No security descriptor available.\n");
		return;
	}
	SECURITY_DESCRIPTOR &sd = *(SECURITY_DESCRIPTOR*)pSD; // to view struct members

	// Get the DACL (Discretionary Access Control List)
	PACL pDACL = NULL;
	BOOL daclPresent, daclDefaulted;
	if (GetSecurityDescriptorDacl(pSD, &daclPresent, &pDACL, &daclDefaulted) && daclPresent && pDACL) 
	{
		int ace_count = pDACL->AceCount;
		_tprintf(_T("  This share-node has %d permission entries(ACEs).\n"), ace_count);

		for (int i = 0; i < ace_count; i++) 
		{
			LPVOID pAce = nullptr;
			if (GetAce(pDACL, i, &pAce)) 
			{
				ACE_HEADER &ace_header = *(ACE_HEADER*)pAce;
				ACCESS_ALLOWED_ACE* pAllowedAce = (ACCESS_ALLOWED_ACE*)pAce;
				PSID pSid = (PSID)&pAllowedAce->SidStart;
				SID &sid = *(SID*)pSid;

				Cec_LocalFree_pstr pszTextSid;
				ConvertSidToStringSid(pSid, &pszTextSid);

				_tprintf(_T("    [ACE %d/%d] on SID %s\n"), i+1, ace_count, (const TCHAR*)pszTextSid);

				print_SidAccountName(sid);
				print_AceType(ace_header.AceType);
				print_AccessMask(pAllowedAce->Mask);
			}
			else
			{
				_tprintf(_T("    Panic! GetAce() fail! WinErr=%d\n"), GetLastError());
			}
		}
	} else {
		printf("No DACL found in security descriptor.\n");
	}
}


enum { Level1=1, Level2=2, Level3=3 };

void test_NetFileEnum()
{
	SHARE_INFO_2 *par_shinfo = NULL;
	DWORD entries_read = 0, entries_total = 0;
	DWORD resume_handle = 0;

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

		print_SharePermissions(shi.shi2_netname);

		_tprintf(_T("\n"));
	}
}


int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	_tprintf(_T("(v1.1)This code calls NetShareEnum() to list all shared-folders.\n"));

	test_NetFileEnum();

	return 0;
}
