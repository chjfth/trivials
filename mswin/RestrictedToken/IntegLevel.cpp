#include <Windows.h>
#include <AclAPI.h>
#include <sddl.h>
#include <assert.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

#include <mswin/WinError.itc.h>
#include <mswin/winnt.itc.h>

struct MapILstr_st
{
	DWORD subauth;
	const TCHAR *pszIL;
} gar_mapLevel[] =
{
	{ SECURITY_MANDATORY_UNTRUSTED_RID, _T("Untrusted0") },
	{ SECURITY_MANDATORY_LOW_RID, _T("Low") },
	{ SECURITY_MANDATORY_MEDIUM_RID, _T("Medium") },
	{ SECURITY_MANDATORY_MEDIUM_PLUS_RID, _T("MediumPlus") },
	{ SECURITY_MANDATORY_HIGH_RID, _T("High") },
	{ SECURITY_MANDATORY_SYSTEM_RID, _T("System") },
	{ SECURITY_MANDATORY_PROTECTED_PROCESS_RID, _T("ProtectedProcess") },
};

const TCHAR *IL2Str(DWORD il)
{
	for(int i=0; i<ARRAYSIZE(gar_mapLevel); i++)
	{
		if(gar_mapLevel[i].subauth==il)
			return gar_mapLevel[i].pszIL;
	}
	return _T("Unknown");
}

DWORD Str2IL(const TCHAR *s)
{
	for(int i=0; i<ARRAYSIZE(gar_mapLevel); i++)
	{
		if( _tcsicmp(gar_mapLevel[i].pszIL, s)==0 )
			return gar_mapLevel[i].subauth;
	}
	return -1;
}

const TCHAR *ILPolicies2Str(DWORD policybits)
{
	static TCHAR s_policystr[80];

	s_policystr[0] = '\0';
	if(policybits & SYSTEM_MANDATORY_LABEL_NO_WRITE_UP)
		_tcscat_s(s_policystr, _T("NO_WRITE_UP|"));
	if (policybits & SYSTEM_MANDATORY_LABEL_NO_READ_UP)
		_tcscat_s(s_policystr, _T("NO_READ_UP|"));
	if (policybits & SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP)
		_tcscat_s(s_policystr, _T("NO_EXECUTE_UP|"));
	
	int slen = (int)_tcslen(s_policystr);
	if(slen>0 && s_policystr[slen-1]=='|')
		s_policystr[slen-1] = '\0';

	if(policybits==0)
	{
		_tcscpy_s(s_policystr, _T("IL-Policy bits from NTFS is zero, weird."));
	}
	else 
	{
		DWORD extra_bits = policybits & ~(
			SYSTEM_MANDATORY_LABEL_NO_WRITE_UP|SYSTEM_MANDATORY_LABEL_NO_READ_UP|SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP
			);
		if(extra_bits)
		{
			_sntprintf_s(s_policystr, _TRUNCATE, _T("%s and extra 0x%0X"), s_policystr,
				extra_bits);
		}
	}

	return s_policystr;
}

const DWORD Str2ILPolicies(const TCHAR *pszPolicies)
{
	DWORD policybits = 0;
	if(_tcsstr(pszPolicies, _T("NO_WRITE_UP"))!=0)
		policybits |= SYSTEM_MANDATORY_LABEL_NO_WRITE_UP;
	if(_tcsstr(pszPolicies, _T("NO_READ_UP"))!=0)
		policybits |= SYSTEM_MANDATORY_LABEL_NO_READ_UP;
	if(_tcsstr(pszPolicies, _T("NO_EXECUTE_UP"))!=0)
		policybits |= SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP;

	if(policybits==0)
	{
		_tprintf(_T("ERROR: Invalid <il-policy> flags input.\n"));
		exit(1);
	}

	return policybits;
}

void show_IntegrityLevel(const TCHAR *szfn)
{
	BOOL succ = 0;
	SECURITY_DESCRIPTOR *pSD = nullptr;
	ACL *pAcl = nullptr;
	SECURITY_INFORMATION siflags = LABEL_SECURITY_INFORMATION; 
	// -- only need this flag for querying Integrity-Level 
	
	DWORD winerr = GetNamedSecurityInfo(szfn, SE_FILE_OBJECT, 
		siflags,
		NULL, NULL, NULL, (PACL*)&pAcl, 
		(PSECURITY_DESCRIPTOR*)&pSD);
	
	if(winerr!=0)
	{
		_tprintf(_T("GetNamedSecurityInfo(\"%s\") fail with winerr=%s\n"),
			szfn, ITCSv(winerr, itc::WinError));
		exit(4);
	}

	DWORD integrityLevel = SECURITY_MANDATORY_MEDIUM_RID;

	if (pAcl && pAcl->AceCount>0)
	{
		assert(1 == pAcl->AceCount); // otherwise, system bug

		SYSTEM_MANDATORY_LABEL_ACE* pAce = 0;

		succ = ::GetAce(pAcl, 0, reinterpret_cast<void**>(&pAce));
		assert(pAce);
		assert(pAce->Header.AceType==SYSTEM_MANDATORY_LABEL_ACE_TYPE);

		SID* psid = reinterpret_cast<SID*>(&pAce->SidStart);

		integrityLevel = psid->SubAuthority[0];
		bool isInherited = (pAce->Header.AceFlags & INHERITED_ACE) ? true : false;

		_tprintf(_T("Integrity-Level: %s (%s)\n"), 
			IL2Str(integrityLevel),
			isInherited ? _T("inherited from parent") : _T("defined on itself, not inherited")
			);
		_tprintf(_T("IL-Policies    : %s\n"), ILPolicies2Str(pAce->Mask));
		_tprintf(_T(".AceFlags      : %s\n"), ITCSv(pAce->Header.AceFlags, itc::AceFlags));
	}
	else
	{
		_tprintf(_T("Integrity Level not set, default to Medium.\n"));
	}

	void *errptr = LocalFree(pSD);
	assert(!errptr);
}

void change_IntegrityLevel(const TCHAR *szfn, DWORD il, DWORD ilpolicies)
{
	const int ACLBUFSIZE = 100; // should be enough
	unsigned char acl[ACLBUFSIZE] = {};
	ACL *pAcl = (ACL*)acl;

	BOOL succ = 0;
	succ = InitializeAcl(pAcl, ACLBUFSIZE, ACL_REVISION);
	assert(succ);

	SID *psidIL = nullptr;
	SID_IDENTIFIER_AUTHORITY authil = SECURITY_MANDATORY_LABEL_AUTHORITY;
	succ = AllocateAndInitializeSid(&authil, 1, 
		il, 0, 0,0,0,0,0,0, (PSID*)&psidIL);
	assert(succ);

	auto pACE = (SYSTEM_MANDATORY_LABEL_ACE*) ((char*)pAcl+sizeof(ACL)); // debug purpose

	succ = AddMandatoryAce(pAcl, ACL_REVISION2, 
		OBJECT_INHERIT_ACE|CONTAINER_INHERIT_ACE /* |NO_PROPAGATE_INHERIT_ACE|INHERIT_ONLY_ACE */,
		ilpolicies,
		psidIL);
	if(!succ)
	{
		_tprintf(_T("AddMandatoryAce() fail with winerr=%d\n"), GetLastError());
		exit(4);
	}

	DWORD winerr = SetNamedSecurityInfo((TCHAR*)szfn, SE_FILE_OBJECT,
		LABEL_SECURITY_INFORMATION,
		NULL, NULL, NULL, 
		pAcl // as into SASL
		);

	if(winerr==0)
	{
		_tprintf(_T("SetNamedSecurityInfo() success, with:\n"));
		_tprintf(_T("    Integrity-Level=%s\n"), IL2Str(il));
		_tprintf(_T("    IL-Policies    =%s\n"), ILPolicies2Str(ilpolicies));
	}
	else
	{
		_tprintf(_T("SetNamedSecurityInfo(\"%s\") fail with winerr=%d\n"),
			szfn, winerr);
		exit(4);
	}

	void *errptr = FreeSid(psidIL);
	assert(!errptr);
}


int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	if(argc==1)
	{
		const TCHAR *s = _T("D:\\test\\foo.txt");
		_tprintf(_T("IntegLevel v1.2, see/change NTFS file/dir's Integrity-level.\n"));
		_tprintf(_T("Missing parameters.\n"));
		_tprintf(_T("    IntegLevel <filepath> [integrity-level] [il-policy]\n"));
		_tprintf(_T("Example:\n"));
		_tprintf(_T("    IntegLevel %s\n"), s);
		_tprintf(_T("    IntegLevel %s Low\n"), s);
		_tprintf(_T("    IntegLevel %s Medium\n"), s);
		_tprintf(_T("    IntegLevel %s High\n"), s);
		_tprintf(_T("    IntegLevel %s Medium NO_WRITE_UP\n"), s);
		_tprintf(_T("    IntegLevel %s Medium NO_WRITE_UP,NO_READ_UP\n"), s);
		_tprintf(_T("    IntegLevel %s Medium NO_EXECUTE_UP\n"), s);
		exit(1);
	}
	const TCHAR *szfn = argv[1];

	if(argc==2)
	{
		show_IntegrityLevel(szfn);
	}
	else
	{
		const TCHAR *ilstr = ilstr = argv[2];
		DWORD il = Str2IL(ilstr);
		if(il==-1)
		{
			_tprintf(_T("ERROR: Invalid Integrity-level input: %s\n"), ilstr);
			exit(4);
		}

		DWORD policybits = SYSTEM_MANDATORY_LABEL_NO_WRITE_UP; // as default
		if(argc>3)
			policybits = Str2ILPolicies(argv[3]);

		change_IntegrityLevel(szfn, il, policybits);
	}

	return 0;
}

