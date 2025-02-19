#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Psapi.h>
#include <setupapi.h>
#include <Cfgmgr32.h>

#include <stdio.h>
#include <Userenv.h>
#include <locale.h>
#include <assert.h>
#include <tchar.h>

#include <mswin/dlptr_winapi.h>
#include <EnsureClnup_mswin.h>

#include "share.h"

int g_sret_len = 0; // success ret
int g_eret_len = 0; // error ret
static DWORD winerr = 0;

TCHAR soutput[MAX_PATH];
TCHAR eoutput[MAX_PATH];

int g_edge_len;
int g_edgeret_len;
TCHAR edge_output[MAX_PATH];

const int SMALL_Usersize = 5;
// -- A small number suitable for most small-buflen API test cases.
//    For a specific test case, you can use this or pick your own.
//    If you pick your own, be sure to define your local var SMALL_Usersize so to
//    override this global one. Example in see_GetSystemDefaultLocaleName().

#define RESET_OUTPUT do { \
	SetLastError(0); \
	g_sret_len = g_eret_len = g_edgeret_len = g_edge_len = FORGOT_INIT; \
	Memset(soutput, BADCHAR, MAX_PATH); \
	Memset(eoutput, BADCHAR, MAX_PATH); \
	Memset(edge_output, BADCHAR, MAX_PATH); \
} while(0)

#define MARK_LEN_NO_REPORT (g_sret_len = g_eret_len = g_edgeret_len = LEN_NO_REPORT)

#define REPORT_API_TRAITS_s(apiname_s) do { \
	ReportTraits( apiname_s, \
		SMALL_Usersize, g_eret_len, winerr, eoutput, \
		g_sret_len, soutput, \
		g_edgeret_len, edge_output ); \
} while(0)

#define REPORT_API_TRAITS(apiname) REPORT_API_TRAITS_s(_T(#apiname))

#define IGNORE_EDGE_CASE TCHAR *edge_output = NULL; g_edgeret_len = g_edge_len = 0;

#define PRN_NO_ANSI_VARIANT(apiname) do { \
	printf("No ANSI variant: " #apiname "()\n"); \
} while(0)

#define cox_CANNOT_RUN_ON_SOME_OS(apiname, dlptr_apiname, osname) \
	if(!dlptr_apiname) { \
		Prn(_T("Not supported on %s: %s()\n"), _T(osname), _T(#apiname)); \
		return; \
	}

#define CANNOT_RUN_ON_WinXP(apisymbol) cox_CANNOT_RUN_ON_SOME_OS(apisymbol, dlptr_ ## apisymbol, "WinXP")
// -- [2025-02-19] Great. I found this solution after I have thoroughly grok the meaning of cohexpand.
//    https://zhuanlan.zhihu.com/p/24638762557

#define OS_WINXP "WinXP"
#define OS_WIN7 "Win7"

//////////////////////////////////////////////////////////////////////////

void see_snprintf_UserBuflenAsMaxfill()
{
	// Note: This function tests the MSVCRT API:

// 	int _sntprintf_s (
// 		TCHAR *outbuf,
// 		size_t buflen,
// 		size_t maxfill,
// 		const TCHAR *format,
// 		...
// 		)

	// It is a bit unusual in that:
	// The user-given buflen does NOT govern the valid buflen, but used as `maxfill`,
	// So we can see this test reports [OverflowNUL] bug.
	// We know that is not a MSVCRT bug, the result of outbuf[maxfill]='\0' is by design.
	//
	// I do it deliberately, just to demonstrate how such a "bug" is detected.

	RESET_OUTPUT;
	errno = 0; // Refer to `errno`, bcz _snprintf is CRT
	const TCHAR *input = _T("0123456789");
	g_sret_len = _sntprintf_s(soutput, ARRAYSIZE(soutput), MAX_PATH,       _T("%s"), input);
	g_eret_len = _sntprintf_s(eoutput, ARRAYSIZE(eoutput), SMALL_Usersize, _T("%s"), input);
	winerr = errno;

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = _sntprintf_s(edge_output, g_edge_len,  _T("%s"), input);

	REPORT_API_TRAITS_s( _T("_snprintf_s(maxfill)") );
}

void see_snprintf_s_TRUNCATE()
{
	RESET_OUTPUT;
	errno = 0; // Refer to `errno`, bcz _snprintf is CRT
	const TCHAR *input = _T("0123456789");
	g_sret_len = _sntprintf_s(soutput, MAX_PATH,       _TRUNCATE, _T("%s"), input);
	g_eret_len = _sntprintf_s(eoutput, SMALL_Usersize, _TRUNCATE, _T("%s"), input);
	winerr = errno;

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = _sntprintf_s(edge_output, g_edge_len, _TRUNCATE, _T("%s"), input);
	
	REPORT_API_TRAITS_s(_T("_snprintf_s(_TRUNCATE)"));
}

void see_GetKeyNameText()
{
	RESET_OUTPUT;
	ULONG key = 0x000e0001; // Backspace
	g_sret_len = GetKeyNameText(key, soutput, MAX_PATH);
	g_eret_len = GetKeyNameText(key, eoutput, SMALL_Usersize);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetKeyNameText(key, edge_output, g_edge_len);

	REPORT_API_TRAITS(GetKeyNameText);
}

void see_GetClassName()
{
	RESET_OUTPUT;
	HWND hwnd = GetDesktopWindow();
	g_sret_len = GetClassName(hwnd, soutput, MAX_PATH); // Usersize-1
	// -- #32769
	g_eret_len = GetClassName(hwnd, eoutput, SMALL_Usersize); // T
	winerr = GetLastError();
	
	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetClassName(hwnd, edge_output, g_edge_len);
	
	REPORT_API_TRAITS(GetClassName);
}

void see_GetWindowText_Explorer()
{
	RESET_OUTPUT;
	HWND hwnd = GetShellWindow();
	g_sret_len = GetWindowText(hwnd, soutput, MAX_PATH); // output "Program Manager"
	g_eret_len = GetWindowText(hwnd, eoutput, SMALL_Usersize);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetWindowText(hwnd, edge_output, g_edge_len);

	REPORT_API_TRAITS_s(_T("GetWindowText(Explorer)"));
}

void see_GetTextFace()
{
	RESET_OUTPUT;
	HDC hdc = GetDC(NULL);

	g_sret_len = GetTextFace(hdc, MAX_PATH, soutput); // Usersize
	// -- "System"
	g_eret_len = GetTextFace(hdc, SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetTextFace(hdc, g_edge_len, edge_output);

	REPORT_API_TRAITS(GetTextFace);

	ReleaseDC(NULL, hdc);
}

void see_GetModuleFileName()
{
	RESET_OUTPUT;
	g_sret_len = GetModuleFileName(NULL, soutput, MAX_PATH);
	// -- D:\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	g_eret_len = GetModuleFileName(NULL, eoutput, SMALL_Usersize);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetModuleFileName(NULL, edge_output, g_edge_len);

	REPORT_API_TRAITS(GetModuleFileName);
}

void see_GetModuleFileName_0buf()
{
	RESET_OUTPUT;
	g_sret_len = GetModuleFileName(NULL, soutput, MAX_PATH);
	// -- D:\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	g_eret_len = GetModuleFileName(NULL, NULL, 0);  
	// -- Note: whan 2nd-param is NULL, 3rd param cannot be a positive value,
	// otherwise, the API will try to write into NULL address.

	winerr = GetLastError();

	IGNORE_EDGE_CASE;

	REPORT_API_TRAITS_s(_T("GetModuleFileName(0buf)"));
}


void see_GetProcessImageFileName()
{
	RESET_OUTPUT;
	g_sret_len = GetProcessImageFileName(GetCurrentProcess(), soutput, MAX_PATH);
	// -- \Device\HarddiskVolume3\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	g_eret_len = GetProcessImageFileName(GetCurrentProcess(), eoutput, SMALL_Usersize);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetProcessImageFileName(GetCurrentProcess(), edge_output, g_edge_len);

	REPORT_API_TRAITS(GetProcessImageFileName);
}

DEFINE_DLPTR_WINAPI("kernel32.dll", QueryFullProcessImageName)

void see_QueryFullProcessImageName() // Vista+
{
	CANNOT_RUN_ON_WinXP(QueryFullProcessImageName);
	//cox_CANNOT_RUN_ON_THIS_OS(QueryFullProcessImageName, OS_WINXP);

	RESET_OUTPUT;
	HANDLE hself = GetCurrentProcess();
	BOOL succ1=0, succ2=0, succ3=0;

	g_sret_len = MAX_PATH;
	succ1 = dlptr_QueryFullProcessImageName(hself, 0, soutput, (DWORD*)&g_sret_len);
	
	g_eret_len = SMALL_Usersize;
	succ2 = dlptr_QueryFullProcessImageName(hself, 0, eoutput, (DWORD*)&g_eret_len);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	succ3 = dlptr_QueryFullProcessImageName(hself, 0, edge_output, (DWORD*)&(g_edgeret_len=g_edge_len));

	REPORT_API_TRAITS(QueryFullProcessImageName);
}

void see_GetSystemDirectory()
{
	RESET_OUTPUT;
	g_sret_len = GetSystemDirectory(soutput, MAX_PATH);
	// -- C:\Windows\system32
	g_eret_len = GetSystemDirectory(eoutput, SMALL_Usersize); // T+1
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetSystemDirectory(edge_output, g_edge_len);

	REPORT_API_TRAITS(GetSystemDirectory);
}

void see_GetWindowsDirectory()
{
	RESET_OUTPUT;
	g_sret_len = GetWindowsDirectory(soutput, MAX_PATH);
	// -- C:\Windows
	g_eret_len = GetWindowsDirectory(eoutput, SMALL_Usersize);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetWindowsDirectory(edge_output, g_edge_len);

	REPORT_API_TRAITS(GetWindowsDirectory);
}

void see_GetCurrentDirectory()
{
	const int SMALL_Usersize = 2; 
	// We need to override this, bcz curdir can be as short as 3 chars, like "C:\"

	RESET_OUTPUT;
	g_sret_len = GetCurrentDirectory(MAX_PATH, soutput);
	// -- D:\gitw\trivials\mswin\seeRetBuf
	g_eret_len = GetCurrentDirectory(SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetCurrentDirectory(g_edge_len, edge_output);

	REPORT_API_TRAITS(GetCurrentDirectory);
}

void see_GetTempPath()
{
	RESET_OUTPUT;
	g_sret_len = GetTempPath(MAX_PATH, soutput);
	// -- C:\Users\win7evn\AppData\Local\Temp\ 
	g_eret_len = GetTempPath(SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetTempPath(g_edge_len, edge_output);

	REPORT_API_TRAITS(GetTempPath);
}

void see_GetEnvironmentVariable()
{
	RESET_OUTPUT;
	TCHAR *envvar = _T("HOMEPATH");
	g_sret_len = GetEnvironmentVariable(envvar, soutput, MAX_PATH);
	// -- \Users\win7evn
	g_eret_len = GetEnvironmentVariable(envvar, eoutput, SMALL_Usersize); // T+1
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetEnvironmentVariable(envvar, edge_output, g_edge_len);

	REPORT_API_TRAITS(GetEnvironmentVariable);
}

void see_GetDefaultUserProfileDirectory()
{
	RESET_OUTPUT;
	g_sret_len = MAX_PATH;
	BOOL succ1 = GetDefaultUserProfileDirectory(soutput, (LPDWORD)&g_sret_len);
	// -- C:\Users\Default
	
	g_eret_len = SMALL_Usersize;
	BOOL succ2 = GetDefaultUserProfileDirectory(eoutput, (LPDWORD)&g_eret_len); // T+1
	// Windows bug: partial filling but missing NUL.
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = g_edge_len;
	BOOL succ3 = GetDefaultUserProfileDirectory(edge_output, (LPDWORD)&g_edgeret_len);

	REPORT_API_TRAITS(GetDefaultUserProfileDirectory);
}

void see_GetLocaleInfo()
{
	RESET_OUTPUT;
	LCID lcid = LOCALE_USER_DEFAULT;
	LCTYPE lctype = LOCALE_SLONGDATE;
	g_sret_len = GetLocaleInfo(lcid, lctype, soutput, MAX_PATH);
	// -- MMMM d, yyyy
	g_eret_len = GetLocaleInfo(lcid, lctype, eoutput, SMALL_Usersize); // 0
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetLocaleInfo(lcid, lctype, edge_output, g_edge_len);

	REPORT_API_TRAITS(GetLocaleInfo);
}

void see_GetLocaleInfo_0buf()
{
	RESET_OUTPUT;
	LCID lcid = LOCALE_USER_DEFAULT;
	LCTYPE lctype = LOCALE_SLONGDATE;
	g_sret_len = GetLocaleInfo(lcid, lctype, soutput, MAX_PATH);
	// -- MMMM d, yyyy
	//	_tprintf(_T("LOCALE_SLONGDATE: %s\n"), soutput);
	g_eret_len = GetLocaleInfo(lcid, lctype, eoutput, 0); // Usersize+1
	winerr = GetLastError();

	IGNORE_EDGE_CASE;

	REPORT_API_TRAITS_s(_T("GetLocaleInfo(0buf)"));
}

DEFINE_DLPTR_WINAPI("kernel32.dll", GetSystemDefaultLocaleName)

void see_GetSystemDefaultLocaleName() // Vista+
{
#ifdef UNICODE
	CANNOT_RUN_ON_WinXP(GetSystemDefaultLocaleName);

	RESET_OUTPUT;

	g_sret_len = dlptr_GetSystemDefaultLocaleName(soutput, MAX_PATH);
	// -- "zh-CN", "en-US", "sr-Latn-XK" etc

	const int SMALL_Usersize = 4;
	g_eret_len = dlptr_GetSystemDefaultLocaleName(eoutput, SMALL_Usersize);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = dlptr_GetSystemDefaultLocaleName(edge_output, g_edge_len);

	REPORT_API_TRAITS(GetSystemDefaultLocaleName);

/*
	This API format: GetSystemDefaultLocaleName(outbuf, buflen);

	v1.2.0, This outputs:

17,GetSystemDefaultLocaleName,Zero,122,No*,T+1,[WackyFill]

	[WackyFill] explanation:

	A typical output from GetSystemDefaultLocaleName() is "en-US" (5 chars), 
	and we need to pass at least buflen=6 to make it success .

	When we pass buflen=4 (or less), this API fills nothing into outbuf[]. But when 
	we pass buflen=5 (still a buffer-too-small case), it fills "en-U\0" into outbuf[],
	so I consider it a inconsistent behavior.

	By and large, this should not cause trouble to API user. 
	But I have not come up a vivid word for this wacky behavior, so just call it a bug.
*/
#else
	PRN_NO_ANSI_VARIANT(GetSystemDefaultLocaleName);
#endif
}

void see_FormatMessage()
{
	RESET_OUTPUT;
	DWORD sampleerr = ERROR_FILE_NOT_FOUND;
	g_sret_len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 
		sampleerr, 
		0, // LANGID
		soutput, MAX_PATH,
		NULL);
	// -- The system cannot find the file specified.
	g_eret_len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 
		sampleerr, 
		0, // LANGID
		eoutput, SMALL_Usersize,
		NULL);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
		sampleerr,
		0, // LANGID
		edge_output, g_edge_len,
		NULL);

	REPORT_API_TRAITS(FormatMessage);
}

#include <InitGuid.h>
DEFINE_GUID(myguid,
	0x20161110, 0x5434, 0x11d3, 0xb8, 0x90, 0x0, 0xc0, 0x4f, 0xad, 0x51, 0x71);

void see_StringFromGUID2()
{
	// Note: StringFromGUID2() has Unicode version only

#ifdef UNICODE
	RESET_OUTPUT;
	g_sret_len = StringFromGUID2(myguid, soutput, MAX_PATH);
	// -- {20161110-5434-11D3-B890-00C04FAD5171}
	g_eret_len = StringFromGUID2(myguid, eoutput, SMALL_Usersize);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = StringFromGUID2(myguid, edge_output, g_edge_len);

	REPORT_API_TRAITS(StringFromGUID2);
#else
	PRN_NO_ANSI_VARIANT(StringFromGUID2);
#endif
}

void see_QueryDosDevice()
{
	RESET_OUTPUT;

	g_sret_len = QueryDosDevice(_T("C:"), soutput, MAX_PATH);
	// -- \Device\HarddiskVolume2

	g_eret_len = QueryDosDevice(_T("C:"), eoutput, SMALL_Usersize);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = QueryDosDevice(_T("C:"), edge_output, g_edge_len);

	REPORT_API_TRAITS(QueryDosDevice);
}

void see_SetupDiGetDeviceInstanceId_CM_Get_Device_ID()
{
	// Try to enumerate all devices, but fetch only the 10th one.
	// I don't fetch the 0-th device, bcz, the 0th device on on VirtualBox 6.1 Win7 VM 
	// happens to be "ROOT\LEGACY_TSSECSRV\0000", that is T+1==26, which is the same value 
	// as CR_BUFFER_SMALL(26).

	const int idxDevice = 10;

	BOOL succ = 0;
	HDEVINFO dis = SetupDiGetClassDevs(NULL, NULL, NULL, DIGCF_ALLCLASSES|DIGCF_PRESENT);
	if(dis==INVALID_HANDLE_VALUE)
	{
		Prn(_T("Unexpect! Error in checking see_SetupDiGetDeviceInstanceId()\n"));
		exit(4);
	}

	SP_DEVINFO_DATA did = {sizeof(did)};
	succ = SetupDiEnumDeviceInfo(dis, idxDevice, &did); 
	if(!succ)
	{
		Prn(_T("Unexpect! Error in checking see_SetupDiGetDeviceInstanceId()\n"));
		exit(4);
	}

	// Check SetupDiGetDeviceInstanceId()
	RESET_OUTPUT;

	DWORD reqsize = 0;
	succ = SetupDiGetDeviceInstanceId(dis, &did, soutput, MAX_PATH, &reqsize);
	assert(succ);
	g_sret_len = reqsize;
	// -- "PCIIDE\IDECHANNEL\4&2617AEAE&0&2"
	//    "ROOT\LEGACY_TDX\0000" (on VirtualBox 6.1 VM)
	//    etc

	reqsize = 0;
	succ = SetupDiGetDeviceInstanceId(dis, &did, eoutput, SMALL_Usersize, &reqsize);
	assert(!succ);
	g_eret_len = reqsize;
	winerr = GetLastError();

	reqsize = 0;
	g_edge_len = STRLEN(soutput);
	succ = SetupDiGetDeviceInstanceId(dis, &did, edge_output, g_edge_len, &reqsize);
	assert(!succ);
	g_edgeret_len = reqsize;

	REPORT_API_TRAITS(SetupDiGetDeviceInstanceId);

	// Check CM_Get_Device_ID_Ex()
	RESET_OUTPUT;
	MARK_LEN_NO_REPORT;

	CONFIGRET cmret = CM_Get_Device_ID(did.DevInst, soutput, MAX_PATH, 0);
	// -- PCIIDE\IDECHANNEL\4&2617AEAE&0&2
	cmret = CM_Get_Device_ID(did.DevInst, eoutput, SMALL_Usersize, 0);
	// -- MSDN: the function supplies as much of the identifier string as will fit into
	// the buffer, and then returns CR_BUFFER_SMALL(26). NOT saying whether fill NUL.
	
	winerr = cmret; 
	// -- CM_xxx does NOT report error code via GetLastError(), so we use cmret instead.

	g_edge_len = STRLEN(soutput);
	CM_Get_Device_ID(did.DevInst, edge_output, g_edge_len, 0);

	REPORT_API_TRAITS(CM_Get_Device_ID);

	SetupDiDestroyDeviceInfoList(dis);
}

void see_GetICMProfile()
{
	RESET_OUTPUT;

	HDC hdc = GetDC(NULL);
	BOOL succ1 = GetICMProfile(hdc, (DWORD*)&(g_sret_len=MAX_PATH), soutput);
	BOOL succ2 = GetICMProfile(hdc, (DWORD*)&(g_eret_len=SMALL_Usersize), eoutput);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	BOOL succ3 = GetICMProfile(hdc, (DWORD*)&(g_edgeret_len=g_edge_len), edge_output);

	REPORT_API_TRAITS(GetICMProfile);

	ReleaseDC(NULL, hdc);
}

MakeDelega_CleanupAny_winapi(Cec_DeleteAtom, ATOM, DeleteAtom, ATOM, 0)

void see_GetAtomName()
{
	RESET_OUTPUT;

	ATOM atom1 = AddAtom(_T("MyAtom1"));
	Cec_DeleteAtom cec_atom1 = atom1;

	g_sret_len = GetAtomName(atom1, soutput, MAX_PATH);
	g_eret_len = GetAtomName(atom1, eoutput, SMALL_Usersize);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetAtomName(atom1, edge_output, g_edge_len);

	REPORT_API_TRAITS(GetAtomName);
}

MakeCleanupClass_winapi(Cec_FindVolumeClose, BOOL, FindVolumeClose, HANDLE, INVALID_HANDLE_VALUE)

void see_FindFirstVolume()
{
	RESET_OUTPUT;
	MARK_LEN_NO_REPORT;

	Cec_FindVolumeClose h1 = FindFirstVolume(soutput, MAX_PATH);
	// --      \\?\Volume{77952243-35b0-11ea-adc6-806e6f6e6963}\

	Cec_FILEHANDLE h2 = FindFirstVolume(eoutput, SMALL_Usersize);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	Cec_FILEHANDLE h3 = FindFirstVolume(edge_output, g_edge_len);
	
	REPORT_API_TRAITS(FindFirstVolume);
}

void see_GetVolumePathNamesForVolumeName()
{
	TCHAR szVolname[250] = {};
	Cec_FindVolumeClose hFindVolume = FindFirstVolume(szVolname, ARRAYSIZE(szVolname));
	// --      \\?\Volume{77952243-35b0-11ea-adc6-806e6f6e6963}\

	if(hFindVolume==INVALID_HANDLE_VALUE)
	{
		Prn(_T("Unexpect! FindFirstVolume() fails with winerr=%d\n"), GetLastError());
		return;
	}

	RESET_OUTPUT;
	const int SMALL_Usersize = 2; // bcz soutput[] may be as short as "D:\"

	for(;;)
	{
		soutput[0] = '\0';
		BOOL succ1 = GetVolumePathNamesForVolumeName(szVolname, soutput, MAX_PATH, (DWORD*)&g_sret_len);
		assert(succ1);
		if(soutput[0]!='\0')
		{
			// soutput[] may typically be "D:\"

			BOOL succ2 = GetVolumePathNamesForVolumeName(szVolname, eoutput, SMALL_Usersize, (DWORD*)&g_eret_len);
			winerr = GetLastError();

			g_edge_len = STRLEN(soutput);
			BOOL succ3 = GetVolumePathNamesForVolumeName(szVolname, edge_output, g_edge_len, (DWORD*)&g_edgeret_len);
			break;
		}

		// Just got an un-mounted volume; Go on finding a mounted one.
		BOOL succ4 = FindNextVolume(hFindVolume, szVolname, ARRAYSIZE(szVolname));
		if(!succ4) // hopefully ERROR_NO_MORE_FILES 
			break;
	}

	REPORT_API_TRAITS(GetVolumePathNamesForVolumeName);
}

void see_GetNumberFormat()
{
	RESET_OUTPUT;

	const TCHAR *szinput = _T("123456789");
	NUMBERFMT nf = {};
	nf.NumDigits = 0;
	nf.LeadingZero = FALSE;
	nf.Grouping = 3;
	nf.lpDecimalSep = TEXT(".");
	nf.lpThousandSep = TEXT(",");
	nf.NegativeOrder = 0;
	
	g_sret_len = GetNumberFormat(LOCALE_USER_DEFAULT, 0, szinput, &nf, soutput, MAX_PATH);
	g_eret_len = GetNumberFormat(LOCALE_USER_DEFAULT, 0, szinput, &nf, eoutput, SMALL_Usersize);
	winerr = GetLastError();

	g_edge_len = STRLEN(soutput);
	g_edgeret_len = GetNumberFormat(LOCALE_USER_DEFAULT, 0, szinput, &nf, edge_output, g_edge_len);
	
	REPORT_API_TRAITS(GetNumberFormat);
}


void check_apis()
{
	see_snprintf_s_TRUNCATE();
	see_snprintf_UserBuflenAsMaxfill();

	see_GetClassName();
	see_GetWindowText_Explorer();

	see_GetKeyNameText();
	see_GetTextFace();

	see_GetModuleFileName();
	see_GetModuleFileName_0buf();

	see_GetProcessImageFileName();
	see_QueryFullProcessImageName();

	see_GetSystemDirectory();
	see_GetWindowsDirectory();
	see_GetCurrentDirectory();

	see_GetTempPath();
	see_GetEnvironmentVariable();

	see_GetDefaultUserProfileDirectory();

	see_GetLocaleInfo();
	see_GetLocaleInfo_0buf();
	see_GetNumberFormat();

	see_GetSystemDefaultLocaleName();

	see_FormatMessage();

	see_StringFromGUID2();

	see_QueryDosDevice();

	see_GetICMProfile();

	see_SetupDiGetDeviceInstanceId_CM_Get_Device_ID();

	see_GetAtomName();
	see_FindFirstVolume();
	see_GetVolumePathNamesForVolumeName();
}
