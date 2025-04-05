#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Aclapi.h>
#include <Sddl.h>
#include <tchar.h>
#include <strsafe.h>
#include <stdio.h>
#include <assert.h>
#include <varargs.h>

#include <itc/InterpretConst.h>
#include <mswin/winnt.itc.h>

#include "ch10-DumpSD.h"

using namespace itc;

const int SDTEXTBUF = 32000;
TCHAR g_dbgbuf[SDTEXTBUF];

#define vaDbgS(szfmt, ...) Append_DumpSD(szfmt, __VA_ARGS__)

static void Append_DumpSD(const TCHAR *szfmt, ...) 
{
	TCHAR tbuf[SDTEXTBUF] = {};

	va_list args;
	va_start(args, szfmt);

	_vsntprintf_s(tbuf, _TRUNCATE, szfmt, args);

	va_end(args);

	_sntprintf_s(g_dbgbuf, _TRUNCATE, _T("%s%s\r\n"), g_dbgbuf, tbuf);
}


#define PSIDFromPACE(pACE) ((PSID)(&((pACE)->SidStart))) // CH10.2

TCHAR * SID2Repr(PSID pvSid, TCHAR buf[], int buflen)
{
	// Convert an SID to text Representation, like:
	// todo: add example 

	if(!pvSid)
	{
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("( none )"));
		return buf;
	}

	if(!IsValidSid(pvSid))
	{
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("( invalid! )"));
		return buf;
	}

	BOOL succ = 0;
	SID *pSid = (SID*)pvSid;
	TCHAR *pszSid = nullptr;

	succ = ConvertSidToStringSid(pvSid, &pszSid);
	assert(succ);


	const int BUFSZ1 = 1024;
	TCHAR szTrusteeNam[BUFSZ1] = {}, szTrusteeDom[BUFSZ1];
	SID_NAME_USE sidType = SidTypeUnknown; // any, as output
	DWORD olen1 = BUFSZ1, olen2 = BUFSZ1;
	succ = LookupAccountSid(NULL, pvSid, szTrusteeNam, &olen1, szTrusteeDom, &olen2, &sidType);

	if(szTrusteeNam[0])
	{
		// corresponding name found.
		assert(succ);

		if(szTrusteeDom[0])	{
			_sntprintf_s(buf, buflen, _TRUNCATE,
				_T("%s ( \"%s\\%s\" )"), pszSid, szTrusteeDom, szTrusteeNam);
		} else {
			_sntprintf_s(buf, buflen, _TRUNCATE,
				_T("%s ( \"%s\" )"), pszSid, szTrusteeNam);
		}

	}
	else
	{
		DWORD winerr = GetLastError();
		assert(!succ && winerr==ERROR_NONE_MAPPED);
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("( no mapped name )"));
	}

	LocalFree(pszSid);
	return buf;
}

TCHAR * ACL2ReprShort(BOOL isPresent, PACL pAcl, TCHAR *buf, int buflen)
{
	if(!isPresent)
	{
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("(none)"));
		return buf;
	}
	else if(!pAcl)
	{
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("(Null-ACL)"));
		return buf;
	}
	else
	{
		_sntprintf_s(buf, buflen, _TRUNCATE, _T("(non-Null)"));
		return buf;
	}

	return nullptr; // Guard for future code-adjusting
}

bool Is_OBJCT_ACE_TYPE(BYTE acetype)
{
	if(acetype>=ACCESS_MIN_MS_OBJECT_ACE_TYPE && acetype<=ACCESS_MAX_MS_OBJECT_ACE_TYPE)
		return true;

	if( acetype==ACCESS_ALLOWED_CALLBACK_OBJECT_ACE_TYPE ||
		acetype==ACCESS_DENIED_CALLBACK_OBJECT_ACE_TYPE ||
		acetype==SYSTEM_AUDIT_CALLBACK_OBJECT_ACE_TYPE ||
		acetype==SYSTEM_ALARM_CALLBACK_OBJECT_ACE_TYPE)
		return true;

	return false;
}

void CH10_DumpACL( PACL pACL, FUNC_InterpretRights *procItr, void *userctx )
{
	// input ACL can be DACL or SACL.
	// This function has special treatment for AceType=SYSTEM_MANDATORY_LABEL_ACE_TYPE
	// as defined by WinSDK doc.

	if (pACL == NULL){
		vaDbgS(TEXT("NULL DACL"));
		return;
	}

	ACL_SIZE_INFORMATION aclSize = {0};
	if (!GetAclInformation(pACL, &aclSize, sizeof(aclSize), AclSizeInformation))
		return;

	vaDbgS(TEXT("ACL ACE count: %d %s"), 
		aclSize.AceCount, 
		aclSize.AceCount==0 ? _T("(=empty ACL)") : _T("")
		);

	for (ULONG lIndex = 0;lIndex < aclSize.AceCount;lIndex++)
	{
		ACCESS_ALLOWED_ACE* pACE = nullptr;

		if (!GetAce(pACL, lIndex, (PVOID*)&pACE))
			return;

		vaDbgS(TEXT("ACE #%d/%d :"), lIndex+1, aclSize.AceCount);

		PSID pSID = nullptr;

		if(!Is_OBJCT_ACE_TYPE(pACE->Header.AceType))
		{
			pSID = PSIDFromPACE(pACE);
		}
		else
		{
			ACCESS_ALLOWED_OBJECT_ACE* pObjACE = (ACCESS_ALLOWED_OBJECT_ACE*)pACE;
			pSID = PSIDFromPACE(pObjACE);
		}
		
		TCHAR szSidRepr[100] = {};
		SID2Repr(pSID, szSidRepr, ARRAYSIZE(szSidRepr));
		vaDbgS(TEXT("  ACE SID = %s"), szSidRepr);

		vaDbgS(TEXT("  ACE Type = %s"), ITCSv(pACE->Header.AceType, AceType));
		vaDbgS(TEXT("  ACE Flags = %s"), ITCSv(pACE->Header.AceFlags, AceFlags));

		if(pACE->Header.AceType==SYSTEM_MANDATORY_LABEL_ACE_TYPE) 
		{	// Vista+: Integrity-level special
			vaDbgS(TEXT("  ACE Mask 0x%X : %s"), 
				pACE->Mask,
				ITCSv(pACE->Mask, SYSTEM_MANDATORY_LABEL_NO_xxx));
		}
		else
		{	// WinXP+: Access-right bits interpretation
			DWORD rights = pACE->Mask;

			TCHAR bitbufs[40] = {};
			ULONG lIndex2 = (ULONG)1<<31;
			for(int i=0, j=0; i<32; i++) {
				bitbufs[j++] = ((rights & lIndex2) != 0)?TEXT('1'):TEXT('0');
				lIndex2 >>= 1;

				if((i+1)%8==0)
					bitbufs[j++] = ' '; // extra space-char every 8 bits
			}

			vaDbgS(TEXT("  ACE Mask 0x%X , (bit 31->0) = %s"), rights, bitbufs);

			if(procItr)
			{
				TCHAR * ptext_to_delete = procItr(rights, userctx);
				vaDbgS(TEXT("%s"), ptext_to_delete);
				delete ptext_to_delete;
			}
		}

	}
}

static bool IsSameBool(BOOL a, BOOL b)
{
	return (a ^ b)==0;
}

void CH10_DumpSD( PSECURITY_DESCRIPTOR pvsd, FUNC_InterpretRights *procItr, void *userctx )
{
	BOOL succ = 0;
	SECURITY_DESCRIPTOR *psd = (SECURITY_DESCRIPTOR*)pvsd;
	const int BUFSIZ1 = 1024;

	// NOTE: SECURITY_DESCRIPTOR's ptr-member(.Owner, .Dacl etc) may be a *offset value*,
	// or a real pointer; this is determined by SE_SELF_RELATIVE(0x8000) flag.
	// To get the real pointers, we have call respective GetSecurityDescriptorXXX APIs.

	PACL pDACL = nullptr, pSACL = nullptr;
	BOOL fPresentDacl=0, fPresentSacl=0;
	BOOL fDefaultedDacl=0, fDefaultedSacl;
	succ = GetSecurityDescriptorDacl(pvsd, &fPresentDacl, &pDACL, &fDefaultedDacl);
	assert( IsSameBool(fPresentDacl, (psd->Control&SE_DACL_PRESENT)?TRUE:FALSE) );
	succ = GetSecurityDescriptorSacl(pvsd, &fPresentSacl, &pSACL, &fDefaultedSacl);
	assert( IsSameBool(fPresentSacl, (psd->Control&SE_SACL_PRESENT)?TRUE:FALSE) );
	//
	PSID psidOwner = nullptr, psidGroup = nullptr;
	BOOL fOwnerDefaulted=0, fGroupDefaulted=0;
	succ = GetSecurityDescriptorOwner(pvsd, &psidOwner, &fOwnerDefaulted);
	assert(succ);
	succ = GetSecurityDescriptorGroup(pvsd, &psidGroup, &fGroupDefaulted);
	assert(succ);

	DWORD sdlen = GetSecurityDescriptorLength(pvsd);

	TCHAR szOwnerRepr[BUFSIZ1]={}, szGroupRepr[BUFSIZ1]={};
	const int BUF20=20; TCHAR szDACL[BUF20]={}, szSACL[BUF20]={};
	vaDbgS(
		_T("SECURITY_DESCRIPTOR at address: 0x%p\r\n")
		_T("  .Revision=%d, length=%d\r\n")
		_T("  .Control(flags) = %s\r\n")
		_T("  OwnerSID = %s\r\n")
		_T("  GroupSID = %s\r\n")
		_T("  DACL: %s ; SACL: %s")
		,
		(void*)psd, psd->Revision, sdlen,
		ITCSv(psd->Control, SE_xxx_sdControl),
		SID2Repr(psidOwner, szOwnerRepr, BUFSIZ1), 
		SID2Repr(psidGroup, szGroupRepr, BUFSIZ1),
		ACL2ReprShort(fPresentDacl, pDACL, szDACL, BUF20), ACL2ReprShort(fPresentSacl, pSACL, szSACL, BUF20)
		);

	vaDbgS(_T("")); // blank line

	if(pDACL)
	{
		vaDbgS(_T("====Dump DACL below:"));
		CH10_DumpACL(pDACL, procItr, userctx);
	}
	if(pSACL)
	{
		vaDbgS(_T("====Dump SACL below:"));
		CH10_DumpACL(pSACL, procItr, userctx);
	}

	vaDbgS(_T("")); // blank line

	// Show string-form Security-descriptor 
	TCHAR *pTextSD = nullptr;
	ULONG nCharsSD = 0;
	succ = ConvertSecurityDescriptorToStringSecurityDescriptor(
		pvsd,
		SDDL_REVISION_1,
		OWNER_SECURITY_INFORMATION|GROUP_SECURITY_INFORMATION|DACL_SECURITY_INFORMATION,
		&pTextSD, &nCharsSD);
	Cec_LocalFree cec_textsd = pTextSD;
	if(succ)
	{
		vaDbgS(_T("Text-form SD:\r\n%s"), pTextSD);
	}
	else
	{
		DWORD winerr = GetLastError();
		vaDbgS(_T("Unexpect! ConvertSecurityDescriptorToStringSecurityDescriptor() error, winerr=%d"), winerr);
	}
}
