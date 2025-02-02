#include <Windows.h>
#include <AclAPI.h>
#include <sddl.h>
#include <assert.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

enum ProtectFlag_et
{
	DontMention = 0, 
	WantProtectBit = 1,
	WantUnprotectBit = 2,
};

void SetNullDacl(const TCHAR *szfn, ProtectFlag_et pf)
{
	SECURITY_INFORMATION siflags = DACL_SECURITY_INFORMATION;
	const TCHAR *pfhint = _T("");

	if(pf==WantProtectBit) {
		siflags |= PROTECTED_DACL_SECURITY_INFORMATION;
		pfhint = _T("(+PROTECT)");
	}
	else if(pf==WantUnprotectBit) {
		siflags |= UNPROTECTED_DACL_SECURITY_INFORMATION;
		pfhint = _T("(-PROTECT)");
	}

	// [2022-11-22] On Win7, even if we add UNPROTECTED_DACL_SECURITY_INFORMATION flag,
	// setting DACL_SECURITY_INFORMATION with pDacl=NULL will make Windows ignore that
	// "unprotect" flag, so "protect" effect is always applied.

	DWORD winerr = SetNamedSecurityInfo((TCHAR*)szfn, SE_FILE_OBJECT,
		siflags, // this is the key
		NULL, NULL, 
		NULL, // pDacl
		NULL);

	if(winerr==0)
	{
		_tprintf(_T("SetNamedSecurityInfo(), set NULL-DACL %s success.\n"), pfhint);
	}
	else
	{
		_tprintf(_T("SetNamedSecurityInfo(\"%s\") fail with winerr=%d\n"),
			szfn, winerr);
		exit(4);
	}
}


int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	if(argc==1)
	{
		const TCHAR *s = _T("D:\\test\\foo.txt");
		_tprintf(_T("This program sets an NTFS file/directory to have NULL-DACL, so to be readable/writable/ownable by EVERYONE.\n"));
		_tprintf(_T("Missing parameters.\n"));
		_tprintf(_T("Example:\n"));
		_tprintf(_T("    NtfsNullDacl %s\n"), s);
		_tprintf(_T("\n"));
		_tprintf(_T("Explicitly add PROTECTED_DACL_SECURITY_INFORMATION flag(exp, no actual effect):\n"));
		_tprintf(_T("    NtfsNullDacl %s +P\n"), s);
		_tprintf(_T("\n"));
		_tprintf(_T("Explicitly add UNPROTECTED_DACL_SECURITY_INFORMATION flag(exp, no actual effect):\n"));
		_tprintf(_T("    NtfsNullDacl %s -P\n"), s);
		exit(1);
	}
	const TCHAR *szfn = argv[1];

	ProtectFlag_et pf = DontMention;
	if(argc>2)
	{
		if(argv[2][0]=='+' && toupper(argv[2][1])=='P')
			pf = WantProtectBit;
		else if(argv[2][0]=='-' && toupper(argv[2][1])=='P')
			pf = WantUnprotectBit;
	}

	SetNullDacl(szfn, pf);

	return 0;
}
