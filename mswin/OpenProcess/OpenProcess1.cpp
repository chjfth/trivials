#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	if(argc==1)
	{
		_tprintf(_T("Need a process id(PID) as parameter.\n"));
		exit(4);
	}
	
	DWORD pid = _ttoi(argv[1]);

	_tprintf(_T("OpenProcess(SYNCHRONIZE|PROCESS_QUERY_INFORMATION, FALSE, %d)...\n"), pid);

	HANDLE hProcess = OpenProcess(SYNCHRONIZE|PROCESS_QUERY_INFORMATION, FALSE, pid);
	if(hProcess)
	{
		_tprintf(_T("Success, hProcess=0x%p. Now wait for target process exit...\n"),
			hProcess);
	}
	else
	{
		_tprintf(_T("Fail. WinErr=%d\n"), GetLastError());
		exit(4);
	}

	DWORD waitre = WaitForSingleObject(hProcess, INFINITE);
	
	if(waitre==WAIT_OBJECT_0)
	{
		DWORD exitcode = -1;
		BOOL succ = GetExitCodeProcess(hProcess, &exitcode);
		if(succ)
			_tprintf(_T("waitre=%d, SUCCESS. His exit-code is %d (0x%X).\n"), waitre, exitcode, exitcode);
		else
			_tprintf(_T("waitre=%d, SUCCESS. But GetExitCodeProcess() fail with winerr=%d.\n"), 
				waitre, GetLastError());
	}
	else
	{
		DWORD winerr = GetLastError();
		_tprintf(_T("waitre=%d, FAIL. WinErr=%d (0x%X)\n"), winerr, winerr);
	}
	
	CloseHandle(hProcess);
	
	return 0;
}
