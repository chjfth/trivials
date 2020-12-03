#include "stdafx.h"

#include <stdio.h>
#include <windows.h>
#include <Wincrypt.h>
#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
void MyHandleError(char *s);
#pragma comment(lib, "crypt32.lib")

typedef struct _ENUM_ARG {
	BOOL        fAll;
	BOOL        fVerbose;
	DWORD       dwFlags; // chj: always 0 in demo code, no use
	const void  *pvStoreLocationPara;
	HKEY        hKeyBase;
} ENUM_ARG, *PENUM_ARG;

//-------------------------------------------------------------------
// Copyright (C) Microsoft.  All rights reserved.
// Declare callback functions. 
// Definitions of these functions follow main.

static BOOL WINAPI EnumPhyCallback(
	const void *pvSystemStore,
	DWORD dwFlags,
	LPCWSTR pwszStoreName,
	PCERT_PHYSICAL_STORE_INFO pStoreInfo,
	void *pvReserved,
	void *pvArg);

static BOOL WINAPI EnumSysCallback(
	const void *pvSystemStore,
	DWORD dwFlags,
	PCERT_SYSTEM_STORE_INFO pStoreInfo,
	void *pvReserved,
	void *pvArg);

static BOOL WINAPI EnumLocCallback(
	LPCWSTR pwszStoreLocation,
	DWORD dwFlags,
	void *pvReserved,
	void *pvArg);

//-------------------------------------------------------------------
// Begin main.

void main(void)
{
	//-------------------------------------------------------------------
	// Declare and initialize variables.

	DWORD dwExpectedError = 0;
//	DWORD dwFlags = 0;
	CERT_PHYSICAL_STORE_INFO PhyStoreInfo;
	ENUM_ARG EnumArg;
	LPSTR pszStoreParameters = NULL;
	LPWSTR pwszStoreParameters = NULL;
	LPWSTR pwszSystemName = NULL;
	LPWSTR pwszPhysicalName = NULL;
	LPWSTR pwszStoreLocationPara = NULL;
	void *pvSystemName;
	void *pvStoreLocationPara;
	DWORD dwNameCnt = 0;
	LPCSTR pszTestName;
	HKEY hKeyRelocate = HKEY_CURRENT_USER;
	LPSTR pszRelocate = NULL;
	HKEY hKeyBase = NULL;

	//-------------------------------------------------------------------
	//  Initialize data structure variables.

	memset(&PhyStoreInfo, 0, sizeof(PhyStoreInfo));
	PhyStoreInfo.cbSize = sizeof(PhyStoreInfo);
	PhyStoreInfo.pszOpenStoreProvider = sz_CERT_STORE_PROV_SYSTEM_W;
	pszTestName = "Enum";
	pvSystemName = pwszSystemName;
	pvStoreLocationPara = pwszStoreLocationPara;

	memset(&EnumArg, 0, sizeof(EnumArg));
	EnumArg.hKeyBase = hKeyBase;

	EnumArg.pvStoreLocationPara = pvStoreLocationPara;
	EnumArg.fAll = TRUE;

	printf("=====================================\n");
	printf("Begin enumeration of store locations.\n");
	printf("=====================================\n");

	if (CertEnumSystemStoreLocation(
		0, // MSDN says this parameter is reserved for CertEnumSystemStoreLocation()
		&EnumArg,
		EnumLocCallback
	))
	{
		printf("\nFinished enumerating locations. \n");
	}
	else
	{
		MyHandleError("Enumeration of locations failed.");
	}

	printf("\n");

	//
	// Chj:
	// Following CertEnumSystemStore() and CertEnumPhysicalStore() is redundant,
	// bcz they have actually been demonstrated inside CertEnumSystemStoreLocation().
	// Following code should produce same partial result from CertEnumSystemStoreLocation().
	// 
	
	DWORD dwFlags = CERT_SYSTEM_STORE_CURRENT_USER;
	
	printf("=============================================================================\n");
	printf("Begin enumeration of system stores(again for CERT_SYSTEM_STORE_CURRENT_USER).\n");
	printf("=============================================================================\n");

	if (CertEnumSystemStore(
		dwFlags,
		pvStoreLocationPara,
		&EnumArg,
		EnumSysCallback
	))
	{
		printf("\nFinished enumerating system stores. \n");
	}
	else
	{
		MyHandleError("Enumeration of system stores failed.");
	}

	printf("\n");
	
	printf("=======================================================\n");
	printf("Enumerate the physical stores for the MY system store. \n");
	printf("=======================================================\n");

	if (CertEnumPhysicalStore(
		L"MY",
		dwFlags,
		&EnumArg,
		EnumPhyCallback
	))
	{
		printf("Finished enumeration of the physical stores. \n");
	}
	else
	{
		MyHandleError("Enumeration of physical stores failed.");
	}
}    //   End of main



	 //-------------------------------------------------------------------
	 //   Define function helper_GetSystemName.

static BOOL helper_GetSystemName(
	const void *pvSystemStore,
	DWORD dwFlags,
	PENUM_ARG pEnumArg,
	LPCWSTR *ppwszSystemName)
{
	//-------------------------------------------------------------------
	// Declare local variables.

	*ppwszSystemName = NULL;

	if (pEnumArg->hKeyBase && 0==(dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG))
	{
		printf("Failed => RELOCATE_FLAG not set in callback. \n");
		return FALSE;
	}
	else
	{
		if (dwFlags & CERT_SYSTEM_STORE_RELOCATE_FLAG)
		{
			PCERT_SYSTEM_STORE_RELOCATE_PARA pRelocatePara;
			if (!pEnumArg->hKeyBase)
			{
				MyHandleError("Failed => RELOCATE_FLAG is set in callback");
			}
			pRelocatePara = (PCERT_SYSTEM_STORE_RELOCATE_PARA)pvSystemStore;
			if (pRelocatePara->hKeyBase != pEnumArg->hKeyBase)
			{
				MyHandleError("Wrong hKeyBase passed to callback");
			}
			*ppwszSystemName = pRelocatePara->pwszSystemStore;
		}
		else
		{
			*ppwszSystemName = (LPCWSTR)pvSystemStore;
		}
	}
	return TRUE;
}

//-------------------------------------------------------------------
// Define the callback functions.

static BOOL WINAPI EnumPhyCallback(
	const void *pvSystemStore,
	DWORD dwFlags,
	LPCWSTR pwszStoreName,
	PCERT_PHYSICAL_STORE_INFO pStoreInfo,
	void *pvReserved,
	void *pvArg)
{
	//-------------------------------------------------------------------
	//  Declare and initialize local variables.
	PENUM_ARG pEnumArg = (PENUM_ARG)pvArg;
	LPCWSTR pwszSystemStore;

	//-------------------------------------------------------------------
	//  Begin callback process.

	if (helper_GetSystemName(
		pvSystemStore,
		dwFlags,
		pEnumArg,
		&pwszSystemStore))
	{
		// [2020-12-03]Chj: Silly code here!
		// The returned pwszSystemStore("My", "Root" etc) above is actually not used here.
		
		// Sample output:
		//	".Default" , ".GroupPolicy" , ".LocalMachine", ...
		//	
		printf("    %S", pwszStoreName);
	}
	else
	{
		MyHandleError("helper_GetSystemName failed.");
	}
	
	if (pEnumArg->fVerbose &&
		(dwFlags & CERT_PHYSICAL_STORE_PREDEFINED_ENUM_FLAG))
	{
		printf(" (implicitly created)");
	}
	
	printf("\n");
	return TRUE;
}

static BOOL WINAPI EnumSysCallback(
	const void *pvSystemStore,
	DWORD dwFlags,
	PCERT_SYSTEM_STORE_INFO pStoreInfo,
	void *pvReserved,
	void *pvArg)
	//-------------------------------------------------------------------
	//  Begin callback process. (Chj: We've got a system-store.)
{
	//-------------------------------------------------------------------
	//  Declare and initialize local variables.

	PENUM_ARG pEnumArg = (PENUM_ARG)pvArg;
	LPCWSTR pwszSystemStore;

	//-------------------------------------------------------------------
	//  Prepare and display the next detail line.

	if (helper_GetSystemName(pvSystemStore, dwFlags, pEnumArg, &pwszSystemStore))
	{
		// Chj: We'll see sth like:
		//	"My", "Root", "Trust", "TrustedPublisher", "AuthRoot", "TrustedPeople", "SmartCardRoot"...
		//	
		printf("  %S\n", pwszSystemStore);
	}
	else
	{
		MyHandleError("helper_GetSystemName failed.");
	}
	
	if (pEnumArg->fAll || pEnumArg->fVerbose)
	{
		dwFlags &= CERT_SYSTEM_STORE_MASK;
		dwFlags |= pEnumArg->dwFlags & ~CERT_SYSTEM_STORE_MASK;
		//
		if (!CertEnumPhysicalStore(
			pvSystemStore, // Chj: tell which system-store to operate on.
			dwFlags,
			pEnumArg,
			EnumPhyCallback
		))
		{
			DWORD dwErr = GetLastError();
			if (!(ERROR_FILE_NOT_FOUND == dwErr ||
				ERROR_NOT_SUPPORTED == dwErr))
			{
				printf("    CertEnumPhysicalStore");
			}
		}
	}
	return TRUE;
}

static BOOL WINAPI EnumLocCallback(
	LPCWSTR pwszStoreLocation,
	DWORD dwFlags,
	void *pvReserved,
	void *pvArg)

{
	//-------------------------------------------------------------------
	//  Declare and initialize local variables.

	PENUM_ARG pEnumArg = (PENUM_ARG)pvArg;

//	DWORD dwLocationID = (dwFlags & CERT_SYSTEM_STORE_LOCATION_MASK) >> CERT_SYSTEM_STORE_LOCATION_SHIFT;
	// Chj: -- looks like no use!

	//-------------------------------------------------------------------
	//  Prepare and display the next detail line.

	printf("======   %S (loc:0x%X)  ======\n", pwszStoreLocation, dwFlags);
	//
	// Output sample: "CurrentUser", "LocalMachine", "CurrentService", "CurrentUserGroupPolicy",
	// "LocalMachineGroupPolicy", "LocalMachineEnterprise" etc.
	
	if (pEnumArg->fAll)
	{
		// Chj: Each time this callback is executed, we see different dwFlags(0x10000, 0x20000, 0x40000, 0x50000,...),
		// so that CertEnumSystemStore() below knows which "location" to operate on.
		
		dwFlags &= CERT_SYSTEM_STORE_MASK;
		dwFlags |= pEnumArg->dwFlags & ~CERT_SYSTEM_STORE_LOCATION_MASK;

		CertEnumSystemStore(
			dwFlags,
			(void *)pEnumArg->pvStoreLocationPara,
			pEnumArg,
			EnumSysCallback);
	}
	return TRUE;
}

//-------------------------------------------------------------------
//  This example uses the function MyHandleError, a simple error
//  handling function, to print an error message to  
//  the standard error (stderr) file and exit the program. 
//  For most applications, replace this function with one 
//  that does more extensive error reporting.

void MyHandleError(char *s)
{
	fprintf(stderr, "An error occurred in running the program. \n");
	fprintf(stderr, "%s\n", s);
	fprintf(stderr, "Error number %x.\n", GetLastError());
	fprintf(stderr, "Program terminating. \n");
	exit(1);
} // End of MyHandleError

