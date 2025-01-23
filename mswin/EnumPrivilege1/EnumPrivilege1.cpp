#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <lm.h>
#include <NTSecAPI.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

// Macro from ntstatus.h
#define STATUS_OBJECT_NAME_NOT_FOUND     ((NTSTATUS)0xC0000034L)

void debuginfo_SID() { SID nullsid = {}; } // let debugger know `SID` struct

BOOL PrintTrusteePrivs(LSA_HANDLE hPolicy, PSID psid)
{
	BOOL fSuccess = FALSE;
	WCHAR szTempPrivBuf[256];
	WCHAR szPrivDispBuf[1024];
	PLSA_UNICODE_STRING plsastrPrivs = NULL;

	__try {
		// Retrieve the array of privileges for the given SID
		ULONG lCount = 0;
		NTSTATUS ntserr = LsaEnumerateAccountRights(hPolicy, psid,
			&plsastrPrivs, &lCount);

		if(ntserr==STATUS_OBJECT_NAME_NOT_FOUND) {
			_tprintf(_T("LsaEnumerateAccountRights() reports STATUS_OBJECT_NAME_NOT_FOUND, so no Privilege directly refers to this trustee.\n"));
			__leave;
		}

		DWORD winerr = LsaNtStatusToWinError(ntserr);
		if (winerr) {
			_tprintf(_T("LsaEnumerateAccountRights() fail. ntserr=0x%X, winerr=%d.\n"),
				ntserr, winerr);
			plsastrPrivs = NULL;
			__leave;
		}

		ULONG lDispLen = 0; ULONG lDispLang = 0;
		for (ULONG lIndex = 0; lIndex < lCount; lIndex++) 
		{
			_sntprintf_s(szTempPrivBuf, _TRUNCATE, L"%.*s", 
				plsastrPrivs[lIndex].Length/sizeof(WCHAR), plsastrPrivs[lIndex].Buffer);

			wprintf(L"%02d. Programmatic Name: %s\n", lIndex+1, szTempPrivBuf);

			// Translate to Display Name
			lDispLen = ARRAYSIZE(szPrivDispBuf); // Size of static Display buffer
			if (LookupPrivilegeDisplayName(NULL, szTempPrivBuf, 
				szPrivDispBuf, &lDispLen, &lDispLang))
				wprintf(L"         Display Name: %s\n\n", szPrivDispBuf);
		}

		fSuccess = TRUE;
	}
	__finally {
		if (plsastrPrivs) 
			LsaFreeMemory(plsastrPrivs);
	}
	return(fSuccess);
}

void test_ListPrivsByUser(const WCHAR *szTrustee)
{
	// szTrustee can be: "Administrator", "Administrators" etc

	LSA_OBJECT_ATTRIBUTES   lsaOA = { sizeof(lsaOA) };
	LSA_HANDLE              hPolicy = NULL;

	// Retrieve the policy handle
	NTSTATUS ntserr = LsaOpenPolicy(
		NULL, // &lsastrComputer, 
		&lsaOA,
		POLICY_VIEW_LOCAL_INFORMATION | POLICY_LOOKUP_NAMES,
		&hPolicy);

	DWORD winerr = LsaNtStatusToWinError(ntserr);
	if (winerr != ERROR_SUCCESS){
		_tprintf(_T("LsaOpenPolicy() fail. ntserr=0x%X, WinErr=%d\n"), ntserr, winerr);
		return;
	}

	// Try to lookup a computer name

	LSA_UNICODE_STRING      lsToQuery = { 0 };
	lsToQuery.Length = (USHORT)(lstrlen(szTrustee)*sizeof(WCHAR));
	lsToQuery.MaximumLength = lsToQuery.Length + sizeof(WCHAR);
	lsToQuery.Buffer = (WCHAR*)szTrustee;

	PLSA_REFERENCED_DOMAIN_LIST pDomainList = NULL;
	// PLSA_TRANSLATED_SID pSidComputer = NULL;
	PLSA_TRANSLATED_SID2 pSidOut = NULL;

	ntserr = LsaLookupNames2(hPolicy, 0,
		1, &lsToQuery, 
		&pDomainList, &pSidOut);
	// -- Check the SID results in debugger: 
	// (DWORD*) ((SID*) pDomainList->Domains[0].Sid)->SubAuthority ,4
	// (DWORD*) ((SID*) pDomainList->Domains[1].Sid)->SubAuthority ,4
	//
	// (SID*)(pSidOut[0].Sid)
	// (DWORD*) ((SID*)pSidOut[0].Sid)->SubAuthority ,5
	// 
	// (SID*)(pSidOut[1].Sid)
	// (DWORD*) ((SID*)pSidOut[1].Sid)->SubAuthority ,5

	if(ntserr)
	{
		winerr = LsaNtStatusToWinError(ntserr);
		_tprintf(_T("LsaLookupNames2(\"%s\") error. ntserr=0x%X, winerr=%d\n"), szTrustee,
			ntserr, winerr);
		return;
	}
		
	PrintTrusteePrivs(hPolicy, pSidOut->Sid);

	ntserr = LsaClose(hPolicy);
	assert(!ntserr);
	ntserr = LsaFreeMemory(pDomainList);
	assert(!ntserr);
	ntserr = LsaFreeMemory(pSidOut);
	assert(!ntserr);
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	if(argc==1)
	{
		printf("Need a username or groupname as parameter.\n");
		return 4;
	}

	test_ListPrivsByUser(argv[1]);
	return 0;
}

