#include <Windows.h>
#include <AclAPI.h>
#include <sddl.h>
#include <assert.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

void SetNullDacl(const TCHAR *szfn, bool is_protect)
{
	SECURITY_INFORMATION siflags = DACL_SECURITY_INFORMATION;

	siflags |= (is_protect ? PROTECTED_DACL_SECURITY_INFORMATION : UNPROTECTED_DACL_SECURITY_INFORMATION);

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
		_tprintf(_T("SetNamedSecurityInfo(), set NULL-DACL %s success.\n"),
			is_protect ? _T("(+PROTECT)" : _T(""))
			);
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
		_tprintf(_T("This program sets an NTFS file/directory to have NULL-DACL, so to be readable/writeable by EVERYONE.\n"));
		_tprintf(_T("Missing parameters.\n"));
		_tprintf(_T("Example:\n"));
		_tprintf(_T("    NtfsNullDacl %s\n"), s);
		_tprintf(_T("Explicitly add PROTECTED_DACL_SECURITY_INFORMATION flag(exp, no actual effect):\n"));
		_tprintf(_T("    NtfsNullDacl %s P\n"), s);
		exit(1);
	}
	const TCHAR *szfn = argv[1];

	bool is_protect = (argc>2 && toupper(argv[2][0])=='P') ? true : false;

	SetNullDacl(szfn, is_protect);

	return 0;
}

