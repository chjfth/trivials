#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winternl.h> // for NtQueryInformationProcess
#include <stdio.h>

#include "\gitw\dailytools\cinclude\dlptr_winapi.h"

DEFINE_DLPTR_WINAPI("ntdll.dll", NtQueryInformationProcess)

#define ProcessDebugPort_7 7 // VC2010 winternl.h does NOT define `ProcessDebugPort`

int main(int argc, char* argv[])
{
	BOOL pres1 = IsDebuggerPresent();

	printf("IsDebuggerPresent() = %d\n", pres1);

	BOOL pres2 = 0;
	BOOL b2 = CheckRemoteDebuggerPresent(GetCurrentProcess(), &pres2);
	if (b2)
	{
		printf("CheckRemoteDebuggerPresent() reports: %s\n",
			pres2 ? "TRUE" : "FALSE");
	}

	DWORD_PTR dbgport = -2;
	ULONG retsize = 0;
	NTSTATUS ntserr = dlptr_NtQueryInformationProcess(
		GetCurrentProcess(),
		PROCESSINFOCLASS(ProcessDebugPort_7),
		&dbgport,
		sizeof(dbgport),
		&retsize);

	if (!ntserr)
		printf("ProcessDebugPort = %d\n", (UINT)dbgport);
	else
		printf("NtQueryInformationProcess() execution fail. ntserr=0x%X\n", ntserr);

	Sleep(500);
	return 0;
}
