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

void test_LowIntegrity_Notepad()
{
	HANDLE hTokenOrigProcess = nullptr;

	BOOL succ = OpenProcessToken(GetCurrentProcess(),
		TOKEN_DUPLICATE, // | TOKEN_ASSIGN_PRIMARY | TOKEN_QUERY,
		&hTokenOrigProcess);
	assert(succ);

	// Prepare a duplicated token so to modify it.

	HANDLE hTokenLowIL = nullptr;
	succ = DuplicateTokenEx(hTokenOrigProcess,
		MAXIMUM_ALLOWED,
		0, // token attributes 
		SecurityAnonymous, // SECURITY_IMPERSONATION_LEVEL
		TokenPrimary,
		&hTokenLowIL);
	assert(succ);

	// Modify the new token to be low-IL.

	unsigned char SIDbuf[sizeof(SID)+sizeof(DWORD)] = {};
	SID *psidLowIL = (SID*)&SIDbuf;
	DWORD sidbytes = sizeof(SIDbuf);
	succ = CreateWellKnownSid(
		WinLowLabelSid, // => SECURITY_MANDATORY_LOW_RID
		NULL, psidLowIL, &sidbytes);
	assert(succ);

	TOKEN_MANDATORY_LABEL tokenLowIL = {};
	tokenLowIL.Label.Sid = psidLowIL;
	tokenLowIL.Label.Attributes = SE_GROUP_INTEGRITY;

	succ = SetTokenInformation(hTokenLowIL, TokenIntegrityLevel, 
		&tokenLowIL, 
		sizeof(TOKEN_MANDATORY_LABEL)
		);

	// Create a new Notepad.exe process with the new IL.

	STARTUPINFO startupInfo = { sizeof (STARTUPINFO) };
	PROCESS_INFORMATION procInfo = {};

	_tprintf(_T("Now I will relaunch a new Notepad.exe process with LOW Intetrity-level.\n"));
	_tprintf(_T("This new Notepad process will NOT let you save files to computer.\n"));

	_tprintf(_T("\n"));
	_tprintf(_T("Press Enter to Continue.\n"));

	_getch();

	_tprintf(_T("\n"));

	succ = CreateProcessAsUser(hTokenLowIL, 
		_T("c:\\windows\\notepad.exe"),
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

	_tprintf(_T("OK. A new Notepad.exe process has been created.\n"));

	succ = CloseHandle(hTokenOrigProcess);
	succ = CloseHandle(hTokenLowIL);

	succ = CloseHandle(procInfo.hThread);
	succ = CloseHandle(procInfo.hProcess);
}


int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	_tprintf(_T("Hello, LowIntegrityNotepad!\n"));

	test_LowIntegrity_Notepad();

	return 0;
}

