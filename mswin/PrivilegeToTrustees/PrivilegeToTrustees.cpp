#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h>
#include <lm.h>
#include <sddl.h>
#include <NTSecAPI.h>

void debuginfo_SID() { SID nullsid = {}; } // let debugger know `SID` struct

LSA_HANDLE get_PolicyHandle(const WCHAR *szComputer=NULL)
{
	LSA_UNICODE_STRING lsComputer = { 0 };
	if(szComputer)
	{
		lsComputer.Length = (USHORT) (lstrlen(szComputer) * sizeof(WCHAR));
		lsComputer.MaximumLength = lsComputer.Length + sizeof(WCHAR);
		lsComputer.Buffer = const_cast<WCHAR*>(szComputer);
	}

	LSA_OBJECT_ATTRIBUTES   lsaOA = { 0 };
	LSA_HANDLE              hPolicy = NULL;

	// Set the size of the useless LSA_OBJECT_ATTRIBUTES structure
	lsaOA.Length = sizeof(lsaOA);

	// Retrieve the policy handle
	NTSTATUS ntserr = LsaOpenPolicy(
		szComputer ? &lsComputer : NULL, 
		&lsaOA,
		POLICY_VIEW_LOCAL_INFORMATION | POLICY_LOOKUP_NAMES,
		&hPolicy);

	if(ntserr)
	{
		DWORD winerr = LsaNtStatusToWinError(ntserr);
		wprintf(L"LsaOpenPolicy() fail. NtsErr=0x%X, WinErr=%d\n", ntserr, winerr);
	}

	return hPolicy;
}

void test_LsaLookupPrivilegeForTrustees(const WCHAR *szPrivi, const WCHAR *szComputer)
{
	LSA_HANDLE hPolicy = get_PolicyHandle(szComputer);

	if(!hPolicy)
		return;

	LSA_UNICODE_STRING lsastrPrivi = { 0 };
	lsastrPrivi.Length = (USHORT)(lstrlen(szPrivi)*sizeof(WCHAR));
	lsastrPrivi.MaximumLength = lsastrPrivi.Length + sizeof(WCHAR);
	lsastrPrivi.Buffer = (WCHAR*)szPrivi;

	PLSA_ENUMERATION_INFORMATION ar_pei = NULL;

	ULONG count = 0; BOOL succ = 0;
	NTSTATUS ntserr = LsaEnumerateAccountsWithUserRight(hPolicy,
		&lsastrPrivi, (void**)&ar_pei, &count);
	// -- (DWORD*)(((SID*)ar_pei[0].Sid)->SubAuthority),2

	if(ntserr)
	{
		DWORD winer = LsaNtStatusToWinError(ntserr);
		wprintf(L"LsaEnumerateAccountsWithUserRight() fail. NtsErr=0x%X, WinErr=%d\n",
			ntserr, winer);
		return;
	}

	for(int i=0; i<(int)count; i++)
	{
		WCHAR *sidstr = NULL;
		succ = ConvertSidToStringSid(ar_pei[i].Sid, &sidstr);

		wprintf(L"[%d]Trustee SID: %s\n", i+1, sidstr);

		HLOCAL isErrHandle = LocalFree(sidstr);

		WCHAR szAccname[400] = {}, szDomain[400] = {};
		DWORD bsAccname = ARRAYSIZE(szAccname), bsDomain = ARRAYSIZE(szDomain);
		SID_NAME_USE sidtype = SidTypeUnknown;
		succ = LookupAccountSid(
			szComputer,  // Chj: should also consult the remote computer for account-names
			ar_pei[i].Sid, 
			szAccname, &bsAccname, szDomain, &bsDomain,
			&sidtype);
		
		wprintf(L"[%d]           : %s @ %s (sidtype=%d)\n", i+1,
			szAccname[0] ? szAccname : L"(?)", 
			szDomain[0] ? szDomain : L"(?)", 
			sidtype);
	}

	ntserr = LsaClose(hPolicy);
	ntserr = LsaFreeMemory(ar_pei);
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	if(argc==1)
	{
		printf("Need one or two params. Example:\n");
		printf("  PrivilegeToTrustees SeIncreaseQuotaPrivilege\n");
		printf("  PrivilegeToTrustees SeIncreaseQuotaPrivilege 10.22.3.239\n");
		return 4;
	}

	test_LsaLookupPrivilegeForTrustees(
		argv[1],
		argc>2 ? argv[2] : NULL
	);
	return 0;
}

/* Sample output:

[1]Trustee SID: S-1-5-32-544
[1]           : Administrators @ BUILTIN (sidtype=4)
[2]Trustee SID: S-1-5-20
[2]           : NETWORK SERVICE @ NT AUTHORITY (sidtype=5)
[3]Trustee SID: S-1-5-19
[3]           : LOCAL SERVICE @ NT AUTHORITY (sidtype=5)

*/