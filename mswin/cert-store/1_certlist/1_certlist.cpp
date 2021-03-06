#include "stdafx.h"

// https://docs.microsoft.com/en-us/windows/win32/seccrypto/example-c-program-listing-the-certificates-in-a-store

#include <cassert>
#include <stdio.h>
#include <windows.h>
#include <wincrypt.h>
#include <cryptuiapi.h>

#include <tchar.h>

#pragma comment (lib, "crypt32.lib")
#pragma comment (lib, "cryptui.lib")

#define MY_ENCODING_TYPE  (PKCS_7_ASN_ENCODING | X509_ASN_ENCODING)
void MyHandleError(char *s);

static void myFormatSystemTime(const SYSTEMTIME &st, char outstr[], int bufchars)
{
	_snprintf_s(outstr, bufchars, _TRUNCATE, "%04d-%02d-%02d %02d:%02d:%02d",
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond
		);
}

void chj_DecodeValidity(const CERT_CONTEXT* pCertContext)
{
	SYSTEMTIME stStart = {}, stEnd={};
	FILETIME localStart, localEnd;

	FileTimeToLocalFileTime(&pCertContext->pCertInfo->NotBefore, &localStart);
	FileTimeToLocalFileTime(&pCertContext->pCertInfo->NotAfter, &localEnd);
	
	FileTimeToSystemTime(&localStart, &stStart);
	FileTimeToSystemTime(&localEnd, &stEnd);

	char szStart[20], szEnd[20];
	myFormatSystemTime(stStart, szStart, _countof(szStart));
	myFormatSystemTime(stEnd, szEnd, _countof(szEnd));
	printf("Validity from %s to %s\n", szStart, szEnd);
}

void chj_DecodeSubject(const CERT_CONTEXT* pCertContext)
{
	char buf1[500];
	int retchars = CertNameToStr(X509_ASN_ENCODING,
		&pCertContext->pCertInfo->Subject,
		CERT_X500_NAME_STR,
		buf1,
		_countof(buf1));
	// buf1[] Sample output:
	//	C=US, S=Washington, L=Redmond, O="Emurasoft, Inc.", OU=SECURE APPLICATION DEVELOPMENT, CN="Emurasoft, Inc."
	
	char decbuf[8000] = {};
	DWORD bufsize = sizeof(decbuf);
	BOOL succ = CryptDecodeObject(MY_ENCODING_TYPE,
		X509_NAME,
		pCertContext->pCertInfo->Subject.pbData,
		pCertContext->pCertInfo->Subject.cbData,
		0,
		decbuf,
		&bufsize
		); // This success.
	CERT_NAME_INFO *pds = (CERT_NAME_INFO*)decbuf;
	//
	printf("Dumping Subject sub-fields:\n");
	for(int i=0; i<(int)pds->cRDN; i++)
	{
		CERT_RDN_ATTR *pattr = pds->rgRDN[i].rgRDNAttr;
		const char *pszfinal = (char*)(pattr->Value.pbData);

		if(pattr->dwValueType==CERT_RDN_UNICODE_STRING || pattr->dwValueType==CERT_RDN_UTF8_STRING)
		{
			printf("  [%s] = %S\n", pattr->pszObjId, pszfinal);
		}
		else
		{
			printf("  [%s] = %s\n", pattr->pszObjId, pszfinal);
		}
	}
}

void main(void)
{

	//-------------------------------------------------------------------
	// Copyright (C) Microsoft.  All rights reserved.
	// This program lists all of the certificates in a system certificate
	// store and all of the property identifier numbers of those 
	// certificates. It also demonstrates the use of two
	// UI functions. One, CryptUIDlgSelectCertificateFromStore, 
	// displays the certificates in a store
	// and allows the user to select one of them, 
	// The other, CryptUIDlgViewContext,
	// displays the contents of a single certificate.

	//-------------------------------------------------------------------
	// Declare and initialize variables.

	HCERTSTORE       hCertStore = NULL;
	PCCERT_CONTEXT   pCertContext = NULL;
	char pszNameString[256] = {};
	char pszStoreName[256] = {};
	void*            pvData;
	DWORD            cbData;

	DWORD            dwPropId = 0;
	// Zero must be used on the first call to the function. After that,
	// the last returned property identifier is passed.

	//-------------------------------------------------------------------
	//  Begin processing and Get the name of the system certificate store 
	//  to be enumerated. Output here is to stderr so that the program  
	//  can be run from the command line and stdout can be redirected  
	//  to a file.

	fprintf(stderr, "Please enter the store name:");
	gets_s(pszStoreName, sizeof(pszStoreName));
	fprintf(stderr, "The store name is %s.\n", pszStoreName);

	//-------------------------------------------------------------------
	// Open a system certificate store.

	if (hCertStore = CertOpenSystemStore(
		NULL,
		pszStoreName))
	{
		fprintf(stderr, "The %s store has been opened. \n", pszStoreName);
	}
	else
	{
		// If the store was not opened, exit to an error routine.
		MyHandleError("The store was not opened.");
	}

	//-------------------------------------------------------------------
	// Use CertEnumCertificatesInStore to get the certificates 
	// from the open store. pCertContext must be reset to
	// NULL to retrieve the first certificate in the store.

	pCertContext = NULL;

	while (pCertContext = CertEnumCertificatesInStore(hCertStore, pCertContext))
	{
		//-------------------------------------------------------------------
		// A certificate was retrieved. Continue.
		//-------------------------------------------------------------------
		//  Display the certificate.

		if (CryptUIDlgViewContext(
			CERT_STORE_CERTIFICATE_CONTEXT,
			pCertContext,
			NULL,
			NULL,
			0,
			NULL))
		{
			printf("CryptUIDlgViewContext() returns OK.\n");
		}
		else
		{
			MyHandleError("UI failed.");
		}

		if (CertGetNameString(
			pCertContext,
			CERT_NAME_SIMPLE_DISPLAY_TYPE,
			0,
			NULL,
			pszNameString,
			128))
		{
			printf("\nCertificate for \"%s\" \n", pszNameString);
		}
		else
			fprintf(stderr, "CertGetName failed. \n");


		chj_DecodeValidity(pCertContext);
		chj_DecodeSubject(pCertContext);


		
		//-------------------------------------------------------------------
		// Loop to find all of the property identifiers for the specified  
		// certificate. The loop continues until 
		// CertEnumCertificateContextProperties returns zero.

		while (dwPropId = CertEnumCertificateContextProperties(
			pCertContext, // The context whose properties are to be listed.
			dwPropId))    // Number of the last property found.  
						  // This must be zero to find the first 
						  // property identifier.
		{
			//-------------------------------------------------------------------
			// When the loop is executed, a property identifier has been found.
			// Print the property number.

			printf("Property #%d found->", dwPropId);

			//-------------------------------------------------------------------
			// Indicate the kind of property found.

			switch (dwPropId)
			{{
			case CERT_FRIENDLY_NAME_PROP_ID:
			{
				printf("CERT_FRIENDLY_NAME_PROP_ID");
				break;
			}
			case CERT_SIGNATURE_HASH_PROP_ID:
			{
				printf("CERT_SIGNATURE_HASH_PROP_ID");
				break;
			}
			case CERT_KEY_PROV_HANDLE_PROP_ID:
			{
				printf("CERT_KEY_PROV_HANDLE_PROP_ID");
				break;
			}
			case CERT_KEY_PROV_INFO_PROP_ID:
			{	// data: CRYPT_KEY_PROV_INFO 
				printf("CERT_KEY_PROV_INFO_PROP_ID");
				break;
			}
			case CERT_SHA1_HASH_PROP_ID:
			{
				printf("CERT_SHA1_HASH_PROP_ID");
				break;
			}
			case CERT_MD5_HASH_PROP_ID:
			{
				printf("CERT_MD5_HASH_PROP_ID");
				break;
			}
			case CERT_KEY_CONTEXT_PROP_ID:
			{
				printf("KEY CONTEXT PROP identifier ");
				break;
			}
			case CERT_KEY_SPEC_PROP_ID:
			{
				printf("CERT_KEY_SPEC_PROP_ID ");
				break;
			}
			case CERT_ENHKEY_USAGE_PROP_ID:
			{
				printf("CERT_ENHKEY_USAGE_PROP_ID");
				break;
			}
			case CERT_NEXT_UPDATE_LOCATION_PROP_ID:
			{
				printf("CERT_NEXT_UPDATE_LOCATION_PROP_ID");
				break;
			}
			case CERT_PVK_FILE_PROP_ID:
			{
				printf("CERT_PVK_FILE_PROP_ID");
				break;
			}
			case CERT_DESCRIPTION_PROP_ID:
			{
				printf("CERT_DESCRIPTION_PROP_ID");
				break;
			}
			case CERT_ACCESS_STATE_PROP_ID:
			{
				printf("CERT_ACCESS_STATE_PROP_ID");
				break;
			}
			case CERT_SMART_CARD_DATA_PROP_ID:
			{
				printf("CERT_SMART_CARD_DATA_PROP_ID");
				break;
			}
			case CERT_EFS_PROP_ID:
			{
				printf("CERT_EFS_PROP_ID");
				break;
			}
			case CERT_FORTEZZA_DATA_PROP_ID:
			{
				printf("CERT_FORTEZZA_DATA_PROP_ID");
				break;
			}
			case CERT_ARCHIVED_PROP_ID:
			{
				printf("CERT_ARCHIVED_PROP_ID");
				break;
			}
			case CERT_KEY_IDENTIFIER_PROP_ID:
			{
				printf("CERT_KEY_IDENTIFIER_PROP_ID");
				break;
			}
			case CERT_AUTO_ENROLL_PROP_ID:
			{
				printf("CERT_AUTO_ENROLL_PROP_ID");
				break;
			}
			case CERT_SUBJECT_PUBLIC_KEY_MD5_HASH_PROP_ID:
			{
				printf("CERT_SUBJECT_PUBLIC_KEY_MD5_HASH_PROP_ID");
				break;
			}
			case CERT_ISSUER_PUBLIC_KEY_MD5_HASH_PROP_ID:
			{
				printf("CERT_ISSUER_PUBLIC_KEY_MD5_HASH_PROP_ID");
				break;
			}
			case CERT_SUBJECT_NAME_MD5_HASH_PROP_ID:
			{
				printf("CERT_SUBJECT_NAME_MD5_HASH_PROP_ID");
				break;
			}
			case CERT_SUBJECT_PUB_KEY_BIT_LENGTH_PROP_ID:
			{
				printf("CERT_SUBJECT_PUB_KEY_BIT_LENGTH_PROP_ID");
				break;
			}
			case CERT_ROOT_PROGRAM_CHAIN_POLICIES_PROP_ID:
			{
				printf("CERT_ROOT_PROGRAM_CHAIN_POLICIES_PROP_ID");
				break;
			}
			case CERT_ROOT_PROGRAM_CERT_POLICIES_PROP_ID:
			{
				printf("CERT_ROOT_PROGRAM_CERT_POLICIES_PROP_ID");
				break;
			}
			case CERT_AUTH_ROOT_SHA256_HASH_PROP_ID:
			{
				printf("CERT_AUTH_ROOT_SHA256_HASH_PROP_ID");
				break;
			}
			case CERT_SIGN_HASH_CNG_ALG_PROP_ID:
			{
				printf("CERT_SIGN_HASH_CNG_ALG_PROP_ID");
				break;
			}

			}} // End switch.

			printf(" (dwPropId=%d) ", dwPropId);
			
			//-------------------------------------------------------------------
			// Retrieve information on the property by first getting the 
			// property size. 
			// For more information, see CertGetCertificateContextProperty.

			if (CertGetCertificateContextProperty(
				pCertContext,
				dwPropId,
				NULL,
				&cbData))
			{
				//  Continue.
			}
			else
			{
				// If the first call to the function failed,
				// exit to an error routine.
				MyHandleError("Call #1 to GetCertContextProperty failed.");
			}
			//-------------------------------------------------------------------
			// The call succeeded. Use the size to allocate memory 
			// for the property.

			if (pvData = (void*)malloc(cbData))
			{
				// Memory is allocated. Continue.
			}
			else
			{
				// If memory allocation failed, exit to an error routine.
				MyHandleError("Memory allocation failed.");
			}
			//----------------------------------------------------------------
			// Allocation succeeded. Retrieve the property data.

			if (CertGetCertificateContextProperty(
				pCertContext,
				dwPropId,
				pvData,
				&cbData))
			{
				// The data has been retrieved. Continue.
				if (dwPropId == CERT_KEY_PROV_INFO_PROP_ID)
				{
					CRYPT_KEY_PROV_INFO *info = (CRYPT_KEY_PROV_INFO*)pvData;
					assert(info);
				}
			}
			else
			{
				// If an error occurred in the second call, 
				// exit to an error routine.
				MyHandleError("Call #2 failed.");
			}
			//---------------------------------------------------------------
			// Show the results.

			printf("The Property Content is @ 0x%p\n", pvData);

			//----------------------------------------------------------------
			// Free the certificate context property memory.

			free(pvData);
		}  // End inner while.
	} // End outer while.

	  //-------------------------------------------------------------------
	  // Select a new certificate by using the user interface.

	if (!(pCertContext = CryptUIDlgSelectCertificateFromStore(
		hCertStore,
		NULL,
		NULL,
		NULL,
		CRYPTUI_SELECT_LOCATION_COLUMN,
		0,
		NULL)))
	{
		MyHandleError("Select UI failed.");
	}


	//-------------------------------------------------------------------
	// Clean up.

	CertFreeCertificateContext(pCertContext);
	CertCloseStore(hCertStore, 0);
	printf("The function completed successfully. \n");
} // End of main.


void MyHandleError(LPTSTR psz)
{
	_ftprintf(stderr, TEXT("An error occurred in the program. \n"));
	_ftprintf(stderr, TEXT("%s\n"), psz);
	_ftprintf(stderr, TEXT("Error number 0x%x.\n"), GetLastError());
	_ftprintf(stderr, TEXT("Program terminating. \n"));
	exit(1);
} // End of MyHandleError.


