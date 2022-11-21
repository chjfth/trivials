#include <Windows.h>
#include <AclAPI.h>
#include <sddl.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

template<typename T1, typename T2>
bool IsSameBool(T1 a, T2 b)
{
	if(a && b)
		return true;
	else if(!a && !b)
		return true;
	else
		return false;
}

void myDisplayNtfsOwner(const TCHAR *szfn)
{
	DWORD succ = 0;
	HANDLE hFile = CreateFile(szfn,
		READ_CONTROL, // dwDesiredAccess=GENERIC_READ etc
		FILE_SHARE_READ|FILE_SHARE_WRITE, // shareMode
		NULL, // SecuAttr, no need bcz we are opening existing file
		OPEN_EXISTING, // dwCreationDisposition
		FILE_FLAG_BACKUP_SEMANTICS, // this is required for open a directory
		NULL);
	if(hFile==INVALID_HANDLE_VALUE)
	{
		_tprintf(_T("Warning: CreateFile() failed!(WinErr=%d) But I will go on calling GetSecurityInfo(0xFFFFffff, ...)\n"),
			GetLastError());
	}

	SECURITY_DESCRIPTOR *pSD = nullptr;
	DWORD winerr = GetSecurityInfo(hFile, SE_FILE_OBJECT, 
		OWNER_SECURITY_INFORMATION,
		NULL, NULL, NULL, NULL,
		(PSECURITY_DESCRIPTOR*)&pSD);
	assert(winerr==0);

	SID* psidOwner = nullptr;
	BOOL isOwnerDefaulted = 0;
	succ = GetSecurityDescriptorOwner(pSD, (PSID*)&psidOwner, &isOwnerDefaulted);
	assert(succ);

	PTSTR strOwner = nullptr;
	succ = ConvertSidToStringSid(psidOwner, &strOwner);
	assert(succ);

	_tprintf(_T("Owner SID is: %s\n"), strOwner);
	_tprintf(_T("Is owner SID defaulted? %s\n"), isOwnerDefaulted?_T("yes"):_T("no"));

	assert(IsSameBool(pSD->Control & SE_OWNER_DEFAULTED, isOwnerDefaulted));

	LocalFree(strOwner);
	LocalFree(pSD);
	CloseHandle(hFile);
}


int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	if(argc==1)
	{
		const TCHAR *s = _T("D:\\test\\foo.txt");
		_tprintf(_T("Missing parameters.\n"));
		_tprintf(_T("Example:\n"));
		_tprintf(_T("    NtfsOwner1 %s\n"), s);
//		_tprintf(_T("    NtfsOwner1 %s -\n"), s);
//		_tprintf(_T("    NtfsOwner1 %s S-1-1-0\n"), s);
		exit(1);
	}
	const TCHAR *szfn = argv[1];

//	const TCHAR *newsid = nullptr;
//	if(argc>2)
//		newsid = argv[2];

	myDisplayNtfsOwner(szfn);	

	return 0;
}

