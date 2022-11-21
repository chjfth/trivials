#include <Windows.h>
#include <AclAPI.h>
#include <sddl.h>
#include <assert.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

struct MapILstr_st
{
	DWORD subauth;
	const TCHAR *pszIL;
} gar_map[] =
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
	for(int i=0; i<ARRAYSIZE(gar_map); i++)
	{
		if(gar_map[i].subauth==il)
			return gar_map[i].pszIL;
	}
	return _T("Unknown");
}

DWORD Str2IL(const TCHAR *s)
{
	for(int i=0; i<ARRAYSIZE(gar_map); i++)
	{
		if( _tcsicmp(gar_map[i].pszIL, s)==0 )
			return gar_map[i].subauth;
	}
	return -1;
}

void show_IntegrityLevel(const TCHAR *szfn)
{
	BOOL succ = 0;
	SECURITY_DESCRIPTOR *pSD = nullptr;
	ACL *pAcl = nullptr;
	DWORD winerr = GetNamedSecurityInfo(szfn, SE_FILE_OBJECT, 
		LABEL_SECURITY_INFORMATION,
		NULL, NULL, NULL, (PACL*)&pAcl, 
		(PSECURITY_DESCRIPTOR*)&pSD);
	
	if(winerr!=0)
	{
		_tprintf(_T("GetNamedSecurityInfo(\"%s\") fail with winerr=%d\n"),
			szfn, winerr);
		exit(4);
	}

	DWORD integrityLevel = SECURITY_MANDATORY_MEDIUM_RID;

	if (pAcl && pAcl->AceCount>0)
	{
		assert(1 == pAcl->AceCount);

		SYSTEM_MANDATORY_LABEL_ACE* ace = 0;

		succ = ::GetAce(pAcl, 0, reinterpret_cast<void**>(&ace));
		assert(ace);

		SID* psid = reinterpret_cast<SID*>(&ace->SidStart);

		integrityLevel = psid->SubAuthority[0];
		bool isInherited = (ace->Header.AceFlags & INHERITED_ACE) ? true : false;

		_tprintf(_T("Integrity Level: %s (%s)\n"), 
			IL2Str(integrityLevel),
			isInherited ? _T("inherited from parent") : _T("defined on itself, not inherited")
			);
	}
	else
	{
		_tprintf(_T("Integrity Level not set, default to Medium.\n"));
	}
}

void change_IntegrityLevel(const TCHAR *szfn, const TCHAR *pszIL)
{
	const int ACLBUFSIZE = 100; // should be enough
	unsigned char acl[ACLBUFSIZE] = {};
	ACL *pAcl = (ACL*)acl;

	DWORD il = Str2IL(pszIL);
	if(il==-1)
	{
		_tprintf(_T("ERROR: Invalid Integrity-level input: %s\n"), pszIL);
		exit(4);
	}

	BOOL succ = 0;
	succ = InitializeAcl(pAcl, ACLBUFSIZE, ACL_REVISION);
	assert(succ);

	SID *psidIL = nullptr;
	SID_IDENTIFIER_AUTHORITY authil = SECURITY_MANDATORY_LABEL_AUTHORITY;
	succ = AllocateAndInitializeSid(&authil, 1, 
		il, 0, 0,0,0,0,0,0, (PSID*)&psidIL);
	assert(succ);

	succ = AddMandatoryAce(pAcl, ACL_REVISION2, 
		OBJECT_INHERIT_ACE|CONTAINER_INHERIT_ACE /* |NO_PROPAGATE_INHERIT_ACE|INHERIT_ONLY_ACE */,
		SYSTEM_MANDATORY_LABEL_NO_WRITE_UP, // SYSTEM_MANDATORY_LABEL_NO_READ_UP, SYSTEM_MANDATORY_LABEL_NO_EXECUTE_UP
		psidIL);
	assert(succ);

	DWORD winerr = SetNamedSecurityInfo((TCHAR*)szfn, SE_FILE_OBJECT,
		LABEL_SECURITY_INFORMATION,
		NULL, NULL, NULL, 
		pAcl // as into SASL
		);

	if(winerr==0)
	{
		_tprintf(_T("SetNamedSecurityInfo() success, with Integrity-Level=%s.\n"), pszIL);
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
		_tprintf(_T("Missing parameters.\n"));
		_tprintf(_T("Example:\n"));
		_tprintf(_T("    IntegLevel %s\n"), s);
		_tprintf(_T("    IntegLevel %s Low\n"), s);
		_tprintf(_T("    IntegLevel %s Medium\n"), s);
		_tprintf(_T("    IntegLevel %s High\n"), s);
		exit(1);
	}
	const TCHAR *szfn = argv[1];

	if(argc==2)
	{
		show_IntegrityLevel(szfn);
	}
	else
	{
		const TCHAR *ilstr = nullptr;
		ilstr = argv[2];
		change_IntegrityLevel(szfn, ilstr);
	}

	return 0;
}

