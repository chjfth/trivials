#define  NOMINMAX // LeWei use?
#define  WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <NTSecAPI.h>
#include <Sddl.h>

#include <EnsureClnup_mswin.h>
#include <mswin/WinError.itc.h>
#include <mswin/NtStatus.itc.h>
#include <mswin/NTSecAPI.itc.h>
using namespace itc;

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Secur32.lib")

#define STATUS_ACCESS_DENIED             ((NTSTATUS)0xC0000022L) // from ntstatus.h


void enum_logon_session_id_detail(LUID* cur)
{
	DWORD count = 0;
	LUID* ptr   = NULL;
	NTSTATUS ntserr = LsaEnumerateLogonSessions(&count, &ptr);
	CEC_LsaFreeReturnBuffer cec_luids = ptr;
	if(ntserr)
	{	// No reason to fail.
		printf("LsaEnumerateLogonSessions() error: ntserr=%s\n", ITCSv(ntserr, NtStatus));
		return;
	}	
	
	printf("Total Logon-Sessions: %u\n", count);

	for (unsigned i = 0; i < count; ++i) 
	{
		LUID& luid = ptr[i];
		const bool matched = (cur && luid.HighPart==cur->HighPart && luid.LowPart==cur->LowPart);

		const char* extra = matched ? " (current session) " : "";
		printf("[%u] %08X-%08X %s", i, luid.HighPart, luid.LowPart, extra);

		SECURITY_LOGON_SESSION_DATA* pp = NULL; 

		ntserr = LsaGetLogonSessionData(&luid, &pp);
		if (!ntserr) 
		{
			printf("\n");

			CEC_LsaFreeReturnBuffer cec_pp = pp;
			SECURITY_LOGON_SESSION_DATA& info = *pp;

			printf("\tUserName: %.*ls\n", info.UserName.Length / 2u, info.UserName.Buffer);
			printf("\tLogonDomain: %.*ls\n", info.LogonDomain.Length / 2u, info.LogonDomain.Buffer);
			printf("\tAuthenticationPackage: %.*ls\n", 
				info.AuthenticationPackage.Length / 2u, info.AuthenticationPackage.Buffer);
			printf("\tLogonType: %s\n", ITCSv(info.LogonType, itc::SECURITY_LOGON_TYPE));
			printf("\tSession-idx: %u\n", info.Session);

			char* str = NULL;
			if (ConvertSidToStringSidA(info.Sid, &str)) 
			{
				printf("\tSid: %s\n", str);
				LocalFree(str);
				str = NULL;
			}

			SYSTEMTIME st = {};
			FILETIME   ft = {};
			FILETIME   locft = {};
			ft.dwHighDateTime  = info.LogonTime.HighPart;
			ft.dwLowDateTime  = info.LogonTime.LowPart;
			if (FileTimeToLocalFileTime(&ft, &locft) && FileTimeToSystemTime(&locft, &st)) 
			{
				printf("\tLogonTime: %04u-%02u-%02u %02u:%02u:%02u\n", 
					st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
			}
		}
		else
		{
			if(ntserr==STATUS_ACCESS_DENIED)
				printf(" (No access right to this session.)\n");
			else
				printf(" (%s)\n", ITCSv(ntserr, NtStatus)); // not likely to see these
		}
	}
	puts("");
}

bool get_logon_session_id(LUID& result)
{
	bool succ = false;

	HANDLE htoken = NULL;

	HANDLE hprocess = OpenProcess(PROCESS_QUERY_INFORMATION, false, GetCurrentProcessId());
	if (hprocess == NULL) return false;

	if (!OpenProcessToken(hprocess, TOKEN_QUERY, &htoken)) 
	{
		CloseHandle(hprocess);
		return false;
	}

	do {
		TOKEN_STATISTICS st = {};
		DWORD retlen = 0;
		if (!GetTokenInformation(htoken, TokenStatistics, &st, sizeof(st), &retlen)) 
			break;
		
		const LUID& luid = st.AuthenticationId; 
		result = luid;
		succ = true;
	} while((void)0, 0);

	CloseHandle(htoken);
	CloseHandle(hprocess);
	return succ;

}

int main(int argc, char *argv[])
{
	printf("seeLogonSessid v1.1\n");

	int cycles = 1;
	if(argc>1)
		cycles = atoi(argv[1]);

	for(int i=0; i<cycles; i++) // Run many times to check for memleak
	{
		LUID luid = {};
		const bool succ = get_logon_session_id(luid);
		enum_logon_session_id_detail(succ ? &luid : NULL);

		if(i>0)
		{
			printf("====Extra %d cycles done.===\n", i);
			printf("\n");
		}
	}

	return 0;
}
