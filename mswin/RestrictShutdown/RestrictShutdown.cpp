#include <Windows.h>
#include <conio.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

// Experiment code from the Web article:
//	Windows Vista for Developers ¨C Part 4 ¨C User Account Control
//	https://weblogs.asp.net/kennykerr/Windows-Vista-for-Developers-_1320_-Part-4-_1320_-User-Account-Control

void do_test()
{
	HANDLE hTokenOrigProcess = nullptr;
	HANDLE hTokenRestricted = nullptr;

	BOOL succ = OpenProcessToken(GetCurrentProcess(),
		TOKEN_DUPLICATE | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY,
		&hTokenOrigProcess);
	assert(succ);

	// Will put `Administrators` (S-1-5-32-544) to disabled-SID list
	//
	unsigned char SIDbuf[sizeof(SID)+sizeof(DWORD)] = {};
	SID *pSIDAdms = (SID*)&SIDbuf;
	DWORD sidbytes = sizeof(SIDbuf);
	succ = CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, pSIDAdms, &sidbytes);
	assert(succ);
	SID_AND_ATTRIBUTES sidsToDisable[] =
	{
		{ pSIDAdms	,0 },
	};

	// Will delete privilege "SeShutdownPrivilege"
	//
	LUID shutdownPrivilege = {};
	succ = LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &shutdownPrivilege);
	assert(succ);

	// Create the restricted-token.
	//
	LUID_AND_ATTRIBUTES privsToDelete[] =
	{
		{ shutdownPrivilege, 0 },
	};
	succ = CreateRestrictedToken(hTokenOrigProcess, 
		0, // flags
		ARRAYSIZE(sidsToDisable), sidsToDisable,
		ARRAYSIZE(privsToDelete), privsToDelete,
		0, 0, // SIDs to restrict
		&hTokenRestricted);
	assert(succ);

	// Create a new Explorer.exe process with the new restricted-token.

	STARTUPINFO startupInfo = { sizeof (STARTUPINFO) };
	PROCESS_INFORMATION procInfo = {};

	_tprintf(_T("Now I will relaunch a new Explorer.exe process with restricted token.\n"));
	_tprintf(_T("This new process will NOT let you shutdown the computer.\n"));

	_tprintf(_T("\n"));
	_tprintf(_T("Press Enter to Continue.\n"));
	
	_getch();

	_tprintf(_T("\n"));

	WinExec("taskkill /F /IM explorer.exe", SW_SHOW);
	Sleep(2000);

	succ = CreateProcessAsUser(hTokenRestricted, 
		_T("c:\\windows\\explorer.exe"),
		0, // cmd line
		0, // process attributes
		0, // thread attributes
		FALSE, // don't inherit handles
		0, // flags
		0, // inherit environment
		0, // inherit current directory
		&startupInfo,
		&procInfo);

	assert(succ);

	_tprintf(_T("OK. a new Explorer.exe process has been created.\n"));

	succ = CloseHandle(hTokenOrigProcess);
	succ = CloseHandle(hTokenRestricted);

	succ = CloseHandle(procInfo.hThread);
	succ = CloseHandle(procInfo.hProcess);
}


int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	_tprintf(_T("Hello, RestrictShutdown!\n"));

	do_test();

	return 0;
}

