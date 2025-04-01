#define  NOMINMAX // LeWei?
#define  WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <NTSecAPI.h>
#include <Sddl.h>


#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Secur32.lib")


void enum_logon_session_id_detail(LUID* cur)
{
	DWORD count = 0;
	LUID* ptr   = NULL;
	if (LsaEnumerateLogonSessions(&count, &ptr) != 0) 
		return;
	printf("Total Logon-Sessions: %u\n", count);

	for (unsigned i = 0; i < count; ++i) 
	{
		LUID& luid = ptr[count - 1 - i];
		const bool matched = (cur && luid.HighPart==cur->HighPart && luid.LowPart==cur->LowPart);

		const char* extra = matched ? " (current session) " : "";
		printf("[%u] %08X-%08X %s\n", i, luid.HighPart, luid.LowPart, extra);

		SECURITY_LOGON_SESSION_DATA* pp = NULL; 
		if (LsaGetLogonSessionData(&luid, &pp) == ERROR_SUCCESS) 
		{
			SECURITY_LOGON_SESSION_DATA& info = *pp;

			static const char* str_type[] = 
			{
				"Interactive",
				"Network",
				"Batch",
				"Service",
				"Proxy",
				"Unlock",
			};
			const unsigned str_type_sz = sizeof(str_type) / sizeof(str_type[0]);
			const unsigned logontype  = info.LogonType;
			const unsigned idx = info.LogonType - Interactive;
			const char* str_logon_type = idx < str_type_sz ? str_type[idx] : "";

			printf("\tUserName: %.*ls\n", info.UserName.Length / 2u, info.UserName.Buffer);
			printf("\tLogonDomain: %.*ls\n", info.LogonDomain.Length / 2u, info.LogonDomain.Buffer);
			printf("\tAuthenticationPackage: %.*ls\n", 
				info.AuthenticationPackage.Length / 2u, info.AuthenticationPackage.Buffer);
			printf("\tLogonType: %u (%s)\n", logontype, str_logon_type);
			printf("\tSession: %u\n", info.Session);

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

			LsaFreeReturnBuffer(pp);

		}
	}
	puts("");
	LsaFreeReturnBuffer(ptr);

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


void enum_logon_session_id()
{
	DWORD count = 0;
	LUID* ptr   = NULL;
	if (LsaEnumerateLogonSessions(&count, &ptr) != 0) return;
	printf("total: %u\n", count);
	for (unsigned i = 0; i < count; ++i) {
		const LUID& luid = ptr[i];
		printf("[%02u] %08X-%08X\n", i, luid.HighPart, luid.LowPart);
	}
	puts("");
	LsaFreeReturnBuffer(ptr);

}

bool show_logon_session_id()
{
	bool succ = false;

	HANDLE htoken = NULL;

	HANDLE hprocess = OpenProcess(PROCESS_QUERY_INFORMATION, false, GetCurrentProcessId());
	if (hprocess == NULL) return false;

	if (!OpenProcessToken(hprocess, TOKEN_QUERY, &htoken)) {
		CloseHandle(hprocess);
		return false;
	}

	do {
		TOKEN_STATISTICS st = {};
		DWORD retlen = 0;
		if (!GetTokenInformation(htoken, TokenStatistics, &st, sizeof(st), &retlen)) break;       
		const LUID& luid = st.AuthenticationId; 
		printf("Logon Session ID: %08X-%08X\n\n", luid.HighPart, luid.LowPart);

		succ = true;
	} while((void)0, 0);

	CloseHandle(htoken);
	CloseHandle(hprocess);
	return succ;

}


int main()
{
	// enum_logon_session_id();
	// show_logon_session_id();

	printf("seeLogonSessid v1.0\n");

	LUID luid = {};
	const bool succ = get_logon_session_id(luid);
	enum_logon_session_id_detail(succ ? &luid : NULL);
	return 0;
}
