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

#include "share.h"

int sret = 0; // success ret
int eret = 0; // error ret
static DWORD winerr = 0;

TCHAR soutput[MAX_PATH];
TCHAR eoutput[MAX_PATH];

int edge_len;
int edge_ret;
TCHAR edge_output[MAX_PATH];

const int SMALL_Usersize = 5;
// -- A small number suitable for most small-buflen API test cases.
//    For a specific test case, you can use this or pick your own.
//    If you pick your own, be sure to define your local var SMALL_Usersize so to
//    override this global one. Example in see_GetSystemDefaultLocaleName().

#define RESET_OUTPUT do { \
	SetLastError(0); \
	sret = eret = edge_ret = edge_len = FORGOT_INIT; \
	Memset(soutput, BADCHAR, MAX_PATH); \
	Memset(eoutput, BADCHAR, MAX_PATH); \
	Memset(edge_output, BADCHAR, MAX_PATH); \
} while(0)


#define REPORT_API_TRAITS_s(apiname_s) do { \
	ReportTraits( apiname_s, \
		SMALL_Usersize, eret, winerr, eoutput, \
		sret, soutput, \
		edge_ret, edge_output ); \
} while(0)

#define REPORT_API_TRAITS(apiname) REPORT_API_TRAITS_s(_T(#apiname))

#define IGNORE_EDGE_CASE TCHAR *edge_output = NULL; edge_len = edge_ret = 0;

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
	sret = _sntprintf_s(soutput, ARRAYSIZE(soutput), MAX_PATH,       _T("%s"), input);
	eret = _sntprintf_s(eoutput, ARRAYSIZE(eoutput), SMALL_Usersize, _T("%s"), input);
	winerr = errno;

	edge_len = STRLEN(soutput);
	edge_ret = _sntprintf_s(edge_output, edge_len,  _T("%s"), input);

	REPORT_API_TRAITS_s( _T("_snprintf_s(maxfill)") );
}

void see_snprintf_s_TRUNCATE()
{
	RESET_OUTPUT;
	errno = 0; // Refer to `errno`, bcz _snprintf is CRT
	const TCHAR *input = _T("0123456789");
	sret = _sntprintf_s(soutput, MAX_PATH,       _TRUNCATE, _T("%s"), input);
	eret = _sntprintf_s(eoutput, SMALL_Usersize, _TRUNCATE, _T("%s"), input);
	winerr = errno;

	edge_len = STRLEN(soutput);
	edge_ret = _sntprintf_s(edge_output, edge_len, _TRUNCATE, _T("%s"), input);
	
	REPORT_API_TRAITS_s(_T("_snprintf_s(_TRUNCATE)"));
}

void see_GetKeyNameText()
{
	RESET_OUTPUT;
	ULONG key = 0x000e0001; // Backspace
	sret = GetKeyNameText(key, soutput, MAX_PATH);
	eret = GetKeyNameText(key, eoutput, SMALL_Usersize);
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = GetKeyNameText(key, edge_output, edge_len);

	REPORT_API_TRAITS(GetKeyNameText);
}

void see_GetClassName()
{
	RESET_OUTPUT;
	HWND hwnd = GetDesktopWindow();
	sret = GetClassName(hwnd, soutput, MAX_PATH); // Usersize-1
	// -- #32769
	eret = GetClassName(hwnd, eoutput, SMALL_Usersize); // T
	winerr = GetLastError();
	
	edge_len = STRLEN(soutput);
	edge_ret = GetClassName(hwnd, edge_output, edge_len);
	
	REPORT_API_TRAITS(GetClassName);
}

void see_GetWindowText_Explorer()
{
	RESET_OUTPUT;
	HWND hwnd = GetShellWindow();
	sret = GetWindowText(hwnd, soutput, MAX_PATH); // output "Program Manager"
	eret = GetWindowText(hwnd, eoutput, SMALL_Usersize);
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = GetWindowText(hwnd, edge_output, edge_len);

	REPORT_API_TRAITS_s(_T("GetWindowText(Explorer)"));
}

void see_GetTextFace()
{
	RESET_OUTPUT;
	HDC hdc = GetDC(NULL);

	sret = GetTextFace(hdc, MAX_PATH, soutput); // Usersize
	// -- "System"
	eret = GetTextFace(hdc, SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = GetTextFace(hdc, edge_len, edge_output);

	REPORT_API_TRAITS(GetTextFace);

	ReleaseDC(NULL, hdc);
}

void see_GetModuleFileName()
{
	RESET_OUTPUT;
	sret = GetModuleFileName(NULL, soutput, MAX_PATH);
	// -- D:\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetModuleFileName(NULL, eoutput, SMALL_Usersize);
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = GetModuleFileName(NULL, edge_output, edge_len);

	REPORT_API_TRAITS(GetModuleFileName);
}

void see_GetModuleFileName_0buf()
{
	RESET_OUTPUT;
	sret = GetModuleFileName(NULL, soutput, MAX_PATH);
	// -- D:\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetModuleFileName(NULL, NULL, 0);  
	// -- Note: whan 2nd-param is NULL, 3rd param cannot be a positive value,
	// otherwise, the API will try to write into NULL address.

	winerr = GetLastError();

	IGNORE_EDGE_CASE;

	REPORT_API_TRAITS_s(_T("GetModuleFileName(0buf)"));
}


void see_GetProcessImageFileName()
{
	RESET_OUTPUT;
	sret = GetProcessImageFileName(GetCurrentProcess(), soutput, MAX_PATH);
	// -- \Device\HarddiskVolume3\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetProcessImageFileName(GetCurrentProcess(), eoutput, SMALL_Usersize);
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = GetProcessImageFileName(GetCurrentProcess(), edge_output, edge_len);

	REPORT_API_TRAITS(GetProcessImageFileName);
}

DEFINE_DLPTR_WINAPI("kernel32.dll", QueryFullProcessImageName)

void see_QueryFullProcessImageName() // Vista+
{
	if(!dlptr_QueryFullProcessImageName) {
		Prn(_T("QueryFullProcessImageName() not supported on WinXP.\n"));
		return;
	}

	RESET_OUTPUT;
	HANDLE hself = GetCurrentProcess();
	BOOL succ1=0, succ2=0, succ3=0;

	sret = MAX_PATH;
	succ1 = dlptr_QueryFullProcessImageName(hself, 0, soutput, (DWORD*)&sret);
	
	eret = SMALL_Usersize;
	succ2 = dlptr_QueryFullProcessImageName(hself, 0, eoutput, (DWORD*)&eret);
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	succ3 = dlptr_QueryFullProcessImageName(hself, 0, edge_output, (DWORD*)&(edge_ret=edge_len));

	REPORT_API_TRAITS(QueryFullProcessImageName);
}

void see_GetSystemDirectory()
{
	RESET_OUTPUT;
	sret = GetSystemDirectory(soutput, MAX_PATH);
	// -- C:\Windows\system32
	eret = GetSystemDirectory(eoutput, SMALL_Usersize); // T+1
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = GetSystemDirectory(edge_output, edge_len);

	REPORT_API_TRAITS(GetSystemDirectory);
}

void see_GetWindowsDirectory()
{
	RESET_OUTPUT;
	sret = GetWindowsDirectory(soutput, MAX_PATH);
	// -- C:\Windows
	eret = GetWindowsDirectory(eoutput, SMALL_Usersize);
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = GetWindowsDirectory(edge_output, edge_len);

	REPORT_API_TRAITS(GetWindowsDirectory);
}

void see_GetCurrentDirectory()
{
	const int SMALL_Usersize = 2; 
	// We need to override this, bcz curdir can be as short as 3 chars, like "C:\"

	RESET_OUTPUT;
	sret = GetCurrentDirectory(MAX_PATH, soutput);
	// -- D:\gitw\trivials\mswin\seeRetBuf
	eret = GetCurrentDirectory(SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = GetCurrentDirectory(edge_len, edge_output);

	REPORT_API_TRAITS(GetCurrentDirectory);
}

void see_GetTempPath()
{
	RESET_OUTPUT;
	sret = GetTempPath(MAX_PATH, soutput);
	// -- C:\Users\win7evn\AppData\Local\Temp\ 
	eret = GetTempPath(SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = GetTempPath(edge_len, edge_output);

	REPORT_API_TRAITS(GetTempPath);
}

void see_GetEnvironmentVariable()
{
	RESET_OUTPUT;
	TCHAR *envvar = _T("HOMEPATH");
	sret = GetEnvironmentVariable(envvar, soutput, MAX_PATH);
	// -- \Users\win7evn
	eret = GetEnvironmentVariable(envvar, eoutput, SMALL_Usersize); // T+1
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = GetEnvironmentVariable(envvar, edge_output, edge_len);

	REPORT_API_TRAITS(GetEnvironmentVariable);
}

void see_GetDefaultUserProfileDirectory()
{
	RESET_OUTPUT;
	sret = MAX_PATH;
	BOOL succ1 = GetDefaultUserProfileDirectory(soutput, (LPDWORD)&sret);
	// -- C:\Users\Default
	
	eret = SMALL_Usersize;
	BOOL succ2 = GetDefaultUserProfileDirectory(eoutput, (LPDWORD)&eret); // T+1
	// Windows bug: partial filling but missing NUL.
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = edge_len;
	BOOL succ3 = GetDefaultUserProfileDirectory(edge_output, (LPDWORD)&edge_ret);

	REPORT_API_TRAITS(GetDefaultUserProfileDirectory);
}

void see_GetLocaleInfo()
{
	RESET_OUTPUT;
	LCID lcid = LOCALE_USER_DEFAULT;
	LCTYPE lctype = LOCALE_SLONGDATE;
	sret = GetLocaleInfo(lcid, lctype, soutput, MAX_PATH);
	// -- MMMM d, yyyy
	eret = GetLocaleInfo(lcid, lctype, eoutput, SMALL_Usersize); // 0
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = GetLocaleInfo(lcid, lctype, edge_output, edge_len);

	REPORT_API_TRAITS(GetLocaleInfo);
}

void see_GetLocaleInfo_0buf()
{
	RESET_OUTPUT;
	LCID lcid = LOCALE_USER_DEFAULT;
	LCTYPE lctype = LOCALE_SLONGDATE;
	sret = GetLocaleInfo(lcid, lctype, soutput, MAX_PATH);
	// -- MMMM d, yyyy
	//	_tprintf(_T("LOCALE_SLONGDATE: %s\n"), soutput);
	eret = GetLocaleInfo(lcid, lctype, eoutput, 0); // Usersize+1
	winerr = GetLastError();

	IGNORE_EDGE_CASE;

	REPORT_API_TRAITS_s(_T("GetLocaleInfo(0buf)"));
}

DEFINE_DLPTR_WINAPI("kernel32.dll", GetSystemDefaultLocaleName)

void see_GetSystemDefaultLocaleName() // Vista+
{
	if(!dlptr_GetSystemDefaultLocaleName) {
		Prn(_T("GetSystemDefaultLocaleName() not supported on WinXP.\n"));
		return;
	}

	RESET_OUTPUT;

	sret = dlptr_GetSystemDefaultLocaleName(soutput, MAX_PATH);
	// -- "zh-CN", "en-US", "sr-Latn-XK" etc

	const int SMALL_Usersize = 4;
	eret = dlptr_GetSystemDefaultLocaleName(eoutput, SMALL_Usersize);
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = dlptr_GetSystemDefaultLocaleName(edge_output, edge_len);

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
}

void see_FormatMessage()
{
	RESET_OUTPUT;
	DWORD sampleerr = ERROR_FILE_NOT_FOUND;
	sret = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 
		sampleerr, 
		0, // LANGID
		soutput, MAX_PATH,
		NULL);
	// -- The system cannot find the file specified.
	eret = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, 
		sampleerr, 
		0, // LANGID
		eoutput, SMALL_Usersize,
		NULL);
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL,
		sampleerr,
		0, // LANGID
		edge_output, edge_len,
		NULL);

	REPORT_API_TRAITS(FormatMessage);
}

#include <InitGuid.h>
DEFINE_GUID(myguid,
	0x20161110, 0x5434, 0x11d3, 0xb8, 0x90, 0x0, 0xc0, 0x4f, 0xad, 0x51, 0x71);

void see_StringFromGUID2()
{
	// Note: StringFromGUID2() has Unicode version only

	RESET_OUTPUT;
	sret = StringFromGUID2(myguid, soutput, MAX_PATH);
	// -- {20161110-5434-11D3-B890-00C04FAD5171}
	eret = StringFromGUID2(myguid, eoutput, SMALL_Usersize);
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = StringFromGUID2(myguid, edge_output, edge_len);

	REPORT_API_TRAITS(StringFromGUID2);
}

void see_QueryDosDevice()
{
	RESET_OUTPUT;

	sret = QueryDosDevice(_T("C:"), soutput, MAX_PATH);
	// -- \Device\HarddiskVolume2

	eret = QueryDosDevice(_T("C:"), eoutput, SMALL_Usersize);
	winerr = GetLastError();

	edge_len = STRLEN(soutput);
	edge_ret = QueryDosDevice(_T("C:"), edge_output, edge_len);

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
	sret = reqsize;
	// -- "PCIIDE\IDECHANNEL\4&2617AEAE&0&2"
	//    "ROOT\LEGACY_TDX\0000" (on VirtualBox 6.1 VM)
	//    etc

	reqsize = 0;
	succ = SetupDiGetDeviceInstanceId(dis, &did, eoutput, SMALL_Usersize, &reqsize);
	assert(!succ);
	eret = reqsize;
	winerr = GetLastError();

	reqsize = 0;
	edge_len = STRLEN(soutput);
	succ = SetupDiGetDeviceInstanceId(dis, &did, edge_output, edge_len, &reqsize);
	assert(!succ);
	edge_ret = reqsize;

	REPORT_API_TRAITS(SetupDiGetDeviceInstanceId);

	// Check CM_Get_Device_ID_Ex()
	RESET_OUTPUT;
	sret = eret = edge_ret = LEN_NO_REPORT;

	CONFIGRET cmret = CM_Get_Device_ID(did.DevInst, soutput, MAX_PATH, 0);
	// -- PCIIDE\IDECHANNEL\4&2617AEAE&0&2
	cmret = CM_Get_Device_ID(did.DevInst, eoutput, SMALL_Usersize, 0);
	// -- MSDN: the function supplies as much of the identifier string as will fit into
	// the buffer, and then returns CR_BUFFER_SMALL(26). NOT saying whether fill NUL.
	
	winerr = cmret; 
	// -- CM_xxx does NOT report error code via GetLastError(), so we use cmret instead.

	edge_len = STRLEN(soutput);
	CM_Get_Device_ID(did.DevInst, edge_output, edge_len, 0);

	REPORT_API_TRAITS(CM_Get_Device_ID);

	SetupDiDestroyDeviceInfoList(dis);
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
	see_GetSystemDefaultLocaleName();

	see_FormatMessage();

	see_StringFromGUID2();

	see_QueryDosDevice();

	see_SetupDiGetDeviceInstanceId_CM_Get_Device_ID();
}
