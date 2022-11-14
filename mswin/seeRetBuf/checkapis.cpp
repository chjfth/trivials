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

#include "share.h"

int sret = 0; // success ret
int eret = 0; // error ret
static DWORD winerr = 0;

TCHAR soutput[MAX_PATH];
TCHAR eoutput[MAX_PATH];

#define SMALL_Usersize 5

#define RESET_OUTPUT do { \
	SetLastError(0); \
	Memset(soutput, BADCHAR, MAX_PATH); \
	Memset(eoutput, BADCHAR, MAX_PATH); \
} while(0)


#define REPORT_API_TRAITS_s(apiname_s) do { \
	ReportTraits( apiname_s, SMALL_Usersize, eret, winerr, eoutput, sret, soutput); \
} while(0)

#define REPORT_API_TRAITS(apiname) REPORT_API_TRAITS_s(_T(#apiname))

void see_snprintf()
{
	RESET_OUTPUT;
	errno = 0; // Refer to `errno`, bcz _snprintf is CRT
	const TCHAR *input = _T("0123456789");
	sret = _sntprintf_s(soutput, MAX_PATH,       _T("%s"), input);
	eret = _sntprintf_s(eoutput, SMALL_Usersize, _T("%s"), input);
	winerr = errno;
	REPORT_API_TRAITS(_snprintf_s);
}

void see_GetKeyNameText()
{
	RESET_OUTPUT;
	LPARAM key = 0x000e0001; // Backspace
	sret = GetKeyNameText(key, soutput, MAX_PATH);
	eret = GetKeyNameText(key, eoutput, SMALL_Usersize);
	winerr = GetLastError();
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
	REPORT_API_TRAITS(GetClassName);
}

void see_GetTextFace()
{
	RESET_OUTPUT;
	HDC hdc = GetDC(NULL);

	sret = GetTextFace(hdc, MAX_PATH, soutput); // Usersize
	// -- System
	eret = GetTextFace(hdc, SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();

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
	REPORT_API_TRAITS(GetModuleFileName);
}

void see_GetModuleFileName_0buf()
{
	RESET_OUTPUT;
	sret = GetModuleFileName(NULL, soutput, MAX_PATH);
	// -- D:\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetModuleFileName(NULL, NULL, 0);
	winerr = GetLastError();
	REPORT_API_TRAITS_s(_T("GetModuleFileName(0buf)"));
}


void see_GetProcessImageFileName()
{
	RESET_OUTPUT;
	sret = GetProcessImageFileName(GetCurrentProcess(), soutput, MAX_PATH);
	// -- \Device\HarddiskVolume3\gitw\trivials\mswin\seeRetBuf\Debug\seeRetBuf.exe
	eret = GetProcessImageFileName(GetCurrentProcess(), eoutput, SMALL_Usersize);
	winerr = GetLastError();
	REPORT_API_TRAITS(GetProcessImageFileName);
}

void see_GetSystemDirectory()
{
	RESET_OUTPUT;
	sret = GetSystemDirectory(soutput, MAX_PATH);
	// -- C:\Windows\system32
	eret = GetSystemDirectory(eoutput, SMALL_Usersize); // T+1
	winerr = GetLastError();
	REPORT_API_TRAITS(GetSystemDirectory);
}

void see_GetWindowsDirectory()
{
	RESET_OUTPUT;
	sret = GetWindowsDirectory(soutput, MAX_PATH);
	// -- C:\Windows
	eret = GetWindowsDirectory(eoutput, SMALL_Usersize);
	winerr = GetLastError();
	REPORT_API_TRAITS(GetWindowsDirectory);
}

void see_GetCurrentDirectory()
{
	RESET_OUTPUT;
	sret = GetCurrentDirectory(MAX_PATH, soutput);
	// -- D:\gitw\trivials\mswin\seeRetBuf
	eret = GetCurrentDirectory(SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();
	REPORT_API_TRAITS(GetCurrentDirectory);
}

void see_GetTempPath()
{
	RESET_OUTPUT;
	sret = GetTempPath(MAX_PATH, soutput);
	// -- C:\Users\win7evn\AppData\Local\Temp\ 
	eret = GetTempPath(SMALL_Usersize, eoutput); // T+1
	winerr = GetLastError();
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
	REPORT_API_TRAITS_s(_T("GetLocaleInfo(0buf)"));
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
	REPORT_API_TRAITS(StringFromGUID2);
}

void see_QueryDosDevice()
{
	RESET_OUTPUT;

	sret = QueryDosDevice(_T("C:"), soutput, MAX_PATH);
	// -- \Device\HarddiskVolume2

	eret = QueryDosDevice(_T("C:"), eoutput, SMALL_Usersize);
	winerr = GetLastError();
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
	// -- PCIIDE\IDECHANNEL\4&2617AEAE&0&2
	reqsize = 0;
	succ = SetupDiGetDeviceInstanceId(dis, &did, eoutput, SMALL_Usersize, &reqsize);
	assert(!succ);
	sret = reqsize;
	winerr = GetLastError();

	REPORT_API_TRAITS(SetupDiGetDeviceInstanceId);

	// Check CM_Get_Device_ID_Ex()
	RESET_OUTPUT;

	CONFIGRET cmret = CM_Get_Device_ID(did.DevInst, soutput, MAX_PATH, 0);
	// -- PCIIDE\IDECHANNEL\4&2617AEAE&0&2
	cmret = CM_Get_Device_ID(did.DevInst, eoutput, SMALL_Usersize, 0);
	// -- MSDN: the function supplies as much of the identifier string as will fit into
	// the buffer, and then returns CR_BUFFER_SMALL(26). NOT saying whether fill NUL.
	
	winerr = cmret; 
	// -- CM_xxx does NOT report error code via GetLastError(), so we use cmret instead.

	REPORT_API_TRAITS(CM_Get_Device_ID);

	SetupDiDestroyDeviceInfoList(dis);
}


void check_apis()
{
	see_snprintf();

	see_GetKeyNameText();
	see_GetClassName();

	see_GetTextFace();

	see_GetModuleFileName();
	see_GetModuleFileName_0buf();

	see_GetProcessImageFileName();

	see_GetSystemDirectory();
	see_GetWindowsDirectory();
	see_GetCurrentDirectory();

	see_GetTempPath();
	see_GetEnvironmentVariable();

	see_GetDefaultUserProfileDirectory();

	see_GetLocaleInfo();
	see_GetLocaleInfo_0buf();

	see_FormatMessage();

	see_StringFromGUID2();

	see_QueryDosDevice();

	see_SetupDiGetDeviceInstanceId_CM_Get_Device_ID();
}
