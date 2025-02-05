#include <windows.h>
#include <sddl.h>
#include <iostream>

using namespace std;

// [2025-02-05] Hinted by GPT4o, with some mods.
// I know I can enumerate all SidTypeAlias groups, from a Windows machine, then how can I enumerate all SidTypeWellKnownGroup groups? The latter includes "Authenticated Users", "CONSOLE LOGON", "LOCAL SERVICE" etc.


void PrintWellKnownSid(WELL_KNOWN_SID_TYPE sidType) 
{
	BYTE sidBuffer[SECURITY_MAX_SID_SIZE];
	DWORD sidSize = sizeof(sidBuffer);

	wcout << L"[#" << sidType <<  L"] Trying WELL_KNOWN_SID_TYPE" << endl;

	if (CreateWellKnownSid(sidType, NULL, sidBuffer, &sidSize)) {
		LPWSTR sidString = NULL;
		if (ConvertSidToStringSidW(sidBuffer, &sidString)) {
			wcout << L"SID: " << sidString << endl;
			LocalFree(sidString);
		}

		WCHAR name[256], domain[256];
		DWORD nameSize = 256, domainSize = 256;
		SID_NAME_USE use;

		if (LookupAccountSidW(NULL, sidBuffer, name, &nameSize, domain, &domainSize, &use)) {
			wcout << L"Name: " << name << L"\n";
		} else {
			wcout << L"Hmm: Name lookup failed with error: " << GetLastError() << endl;
		}
	} else {
		wcout << L"BAD: Failed to create SID for type " << sidType << endl;
	}
}

int main() {
	for (int i = WinNullSid; i <= WinThisOrganizationCertificateSid; i++) {
		PrintWellKnownSid(static_cast<WELL_KNOWN_SID_TYPE>(i));

		wcout << endl;
	}
	return 0;
}
