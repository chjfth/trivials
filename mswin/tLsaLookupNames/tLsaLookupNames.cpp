#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <lm.h>
#include <NTSecAPI.h>
#include <SDDL.h>
#include <ShellAPI.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include "resource.h"

#include "iversion.h"

#include "../utils.h"
#include <EnsureClnup_mswin.h>

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


// Macro from ntstatus.h
#define STATUS_OBJECT_NAME_NOT_FOUND     ((NTSTATUS)0xC0000034L)
#define STATUS_SOME_NOT_MAPPED           ((NTSTATUS)0x00000107L) // partial success

void debuginfo_SID() { SID nullsid = {}; } // let debugger know `SID` struct


MakeCleanupCxxClass(LSA_UNICODE_STRING)

MakeCleanupPtrClass_winapi(Cec_LsaClose, NTSTATUS, LsaClose, LSA_HANDLE)

MakeCleanupPtrClass_winapi(Cec_LsaFreeMemory, NTSTATUS, LsaFreeMemory, PVOID)

MakeCleanupPtrClass_winapi(Cec_LocalFree, HLOCAL, LocalFree, HLOCAL)

MakeCleanupPtrClass_winapi(Cec_FreeSid, PVOID, FreeSid, PSID) // not used yet

HINSTANCE g_hinstExe;

int g_docycles = 1; // Use to test memory leak, set by command-line, eg 10000 

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

struct MapInt2Str {
	int n;
	const TCHAR *s;
};

#define MAP_ENTRY(i) { i, _T(#i) }

const TCHAR *getSidType(SID_NAME_USE use)
{
	MapInt2Str maps[] = 
	{
		MAP_ENTRY(SidTypeUser),
		MAP_ENTRY(SidTypeGroup),
		MAP_ENTRY(SidTypeDomain),
		MAP_ENTRY(SidTypeAlias),
		MAP_ENTRY(SidTypeWellKnownGroup),
		MAP_ENTRY(SidTypeDeletedAccount),
		MAP_ENTRY(SidTypeInvalid),
		MAP_ENTRY(SidTypeUnknown),
		MAP_ENTRY(SidTypeComputer),
		MAP_ENTRY(SidTypeLabel),
	};
	for(int i=0; i<_countof(maps); i++)
	{
		if(maps[i].n==use)
			return maps[i].s;
	}
	return _T("Bad SID_NAME_USE value");
}

bool in_LsaLookupNames(HWND hdlg, LSA_UNICODE_STRING *arus, int uscount)
{
	LSA_OBJECT_ATTRIBUTES   lsaOA = { sizeof(lsaOA) };
	LSA_HANDLE              hPolicy = NULL;

	// Retrieve the policy handle
	NTSTATUS ntserr = LsaOpenPolicy(
		NULL, // &lsastrComputer, 
		&lsaOA,
		POLICY_VIEW_LOCAL_INFORMATION | POLICY_LOOKUP_NAMES,
		&hPolicy);
	Cec_LsaClose cec_hPolicy = hPolicy; // ensure cleanup hPolicy

	DWORD winerr = LsaNtStatusToWinError(ntserr);
	if (winerr != ERROR_SUCCESS){
		vaMsgBox(hdlg, MB_OK|MB_ICONWARNING, _T("Error"),
			_T("LsaOpenPolicy() fail. ntserr=0x%X, WinErr=%d\n"), ntserr, winerr);
		return false;
	}


	PLSA_REFERENCED_DOMAIN_LIST pxxDomainList = NULL;
	PLSA_TRANSLATED_SID2 pxSidOut = NULL;

	ntserr = LsaLookupNames2(hPolicy, 0,
		uscount, arus, 
		&pxxDomainList, &pxSidOut);
	// -- Check the SID results in debugger: 
	// (DWORD*) ((SID*) pxxDomainList->Domains[0].Sid)->SubAuthority ,4
	// (DWORD*) ((SID*) pxxDomainList->Domains[1].Sid)->SubAuthority ,4
	// 
	// (SID*)(pxSidOut[0].Sid)
	// (DWORD*) ((SID*)pxSidOut[0].Sid)->SubAuthority ,5
	// 
	// (SID*)(pxSidOut[1].Sid)
	// (DWORD*) ((SID*)pxSidOut[1].Sid)->SubAuthority ,5

	Cec_LsaFreeMemory cec_domainlist = pxxDomainList;
	Cec_LsaFreeMemory cec_sidout = pxSidOut;

	if(ntserr && ntserr!=STATUS_SOME_NOT_MAPPED)
	{
		winerr = LsaNtStatusToWinError(ntserr);

		if(winerr==ERROR_NONE_MAPPED) {
			vaMsgBox(hdlg, MB_OK|MB_ICONWARNING, _T("Error"),
				_T("WinErr=%d, No input names queried success."), winerr);
		} else {
			vaMsgBox(hdlg, MB_OK|MB_ICONWARNING, _T("Error"),
				_T("LsaLookupNames2() error. ntserr=0x%X, winerr=%d\n"), ntserr, winerr);
		}
		return false;
	}

	//
	// Extract members in xSidOut
	//

	const int BUFSIZE = 16000;
	TCHAR textbuf[BUFSIZE] = {};

	int i;
	for(i=0; i<uscount; i++)
	{
		LSA_TRANSLATED_SID2 &xsid = pxSidOut[i];

		_sntprintf_s(textbuf, _TRUNCATE, _T("%s[#%d] %.*s\r\n"), textbuf, 
			i, 
			arus[i].Length/sizeof(TCHAR), arus[i].Buffer);

		if(xsid.Use==SidTypeUnknown || xsid.Use==SidTypeInvalid)
		{
			_sntprintf_s(textbuf, _TRUNCATE, _T("%s###BAD### Cannot find this trustee name!\r\n"), textbuf);
			
			// [2025-01-22] Chj: But still try to show xsid's other members.
			// We'll see that, xsid.DomainIndex can be -1, or a stale value (try "BUILTIN\baduser") .
		}
	
		_sntprintf_s(textbuf, _TRUNCATE, _T("%s%s (=%d)\r\n"), textbuf,
			getSidType(xsid.Use) , xsid.Use);

		_sntprintf_s(textbuf, _TRUNCATE, _T("%sDomainIndex=%d\r\n"), textbuf,
			xsid.DomainIndex);

		TCHAR *pSidText = NULL;
		ConvertSidToStringSid(xsid.Sid, &pSidText);
		Cec_LocalFree cec_sidtext = pSidText;
	
		_sntprintf_s(textbuf, _TRUNCATE, _T("%sSID=%s\r\n"), textbuf,
			pSidText);

		_sntprintf_s(textbuf, _TRUNCATE, _T("%s\r\n"), textbuf); // extra blank line
	}

	SetDlgItemText(hdlg, IDC_EDIT_SidOutput, textbuf);

	//
	// Extract members in xxDomainList
	//

	textbuf[0] = '\0';

	for(i=0; i<(int)pxxDomainList->Entries; i++)
	{
		LSA_TRUST_INFORMATION &xDomain = pxxDomainList->Domains[i];

		LSA_UNICODE_STRING &us = xDomain.Name;
		
		_sntprintf_s(textbuf, _TRUNCATE, _T("%s[#%d] %.*s\r\n"), textbuf,
			i,
			us.Length/sizeof(TCHAR), us.Buffer);

		TCHAR *pSidText = NULL;
		ConvertSidToStringSid(xDomain.Sid, &pSidText);
		Cec_LocalFree cec_sidtext = pSidText;

		_sntprintf_s(textbuf, _TRUNCATE, _T("%sSID=%s\r\n"), textbuf,
			pSidText);
		
		_sntprintf_s(textbuf, _TRUNCATE, _T("%s\r\n"), textbuf); // extra blank line
	}

	SetDlgItemText(hdlg, IDC_EDIT_DomainOutput, textbuf);

	return true;
}


static int count_lines(const TCHAR *str)
{
	if(!str || !str[0])
		return 0;

	int i = 0, count = 1;
	for(; str[i]; i++)
	{
		if(str[i]=='\n')
			count++;
	}
	
	if(str[i-1]=='\n')
		count--;

	return count;
}

bool do_LsaLookupNames(HWND hdlg)
{
	TCHAR szinput[4000] = {};
	GetDlgItemText(hdlg, IDC_EDIT_Input, szinput, _countof(szinput)-1);

	SetDlgItemText(hdlg, IDC_EDIT_SidOutput, _T(""));
	SetDlgItemText(hdlg, IDC_EDIT_DomainOutput, _T(""));

	int nlines = count_lines(szinput);
	if(nlines<=0)
	{
		vaMsgBox(hdlg, MB_OK, _T("Info"), _T("Empty input. Nothing to do."));
		return false;
	}

	// Build LSA_UNICODE_STRING array of nlines eles
	
	CecArray_LSA_UNICODE_STRING arus = new LSA_UNICODE_STRING[nlines];
	int i = 0, iline = 0;
	while( szinput[i] )
	{
		// Skip any \r\n :
		while(szinput[i]=='\r' || szinput[i]=='\n')
			i++;

		if(szinput[i]=='\0')
			break;

		// now at a line start

		arus[iline].Buffer = szinput+i;
		
		// Find line-end by scanning for \r or \n
		int j = i;
		while(szinput[j]!='\r' && szinput[j]!='\n' && szinput[j]!='\0')
			j++;

		arus[iline].Length = USHORT((j - i) * sizeof(TCHAR)); // length in bytes
		arus[iline].MaximumLength = arus[iline].Length;

		iline++;

		i = j;
	}

	vaDbgS(_T("Input %d names:"), iline);
	for(i=0; i<iline; i++)
		vaDbgS(_T("    %.*s"), arus[i].Length/sizeof(TCHAR), arus[i].Buffer);

	DWORD msec_start = TrueGetMillisec();

	bool succ = false;
	for(i=0; i<g_docycles; i++)
	{
		vaSetDlgItemText(hdlg, IDC_LBL_DoCycles, _T("DoCycle: %d/%d"),
			i, g_docycles);

		succ = in_LsaLookupNames(hdlg, arus, iline);
		if(!succ)
			break;
	}

	DWORD msec_end = TrueGetMillisec();
	DWORD msec_used = msec_end - msec_start; 
	
	if(g_docycles==1)
	{
		vaSetDlgItemText(hdlg, IDC_LBL_DoCycles, _T("Time used: %u millisec"), msec_used);
	}
	else
	{
		vaSetDlgItemText(hdlg, IDC_LBL_DoCycles, _T("%d cycles: %u millisec"),
			g_docycles, msec_used);
	}
	
	return succ;
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{

	switch (id) 
	{{
	case IDC_BTN_LOOKUP:
	{
		do_LsaLookupNames(hdlg);
//		InvalidateRect(GetDlgItem(hdlg, IDC_LABEL1), NULL, TRUE);
		break;
	}
	case IDOK:
	case IDCANCEL:
	{
		EndDialog(hdlg, id);
		break;
	}
	}}
}

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg, 440, 0); // min-width=440

	jul->AnchorControl(0,0, 0,100, IDC_EDIT_Input);
	jul->AnchorControl(0,100, 0, 100, IDC_LBL_DoCycles);
	jul->AnchorControl(0,100, 0,100, IDC_BTN_LOOKUP);
	
	const int div = 70;
	jul->AnchorControl(0,0, 100,div, IDC_EDIT_SidOutput);
	jul->AnchorControl(0,div, 100,div, IDC_LABEL_DomainOutput);
	jul->AnchorControl(0,div, 100,100, IDC_EDIT_DomainOutput);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	SetDlgItemText(hdlg, IDC_LBL_DoCycles, _T(""));

	vaSetWindowText(hdlg, _T("tLsaLookupNames version: %d.%d.%d"), 
		tLsaLookupNames_VMAJOR, tLsaLookupNames_VMINOR, tLsaLookupNames_VPATCH);

	SetDlgItemText(hdlg, IDC_EDIT_Input, 
		_T("Everyone\r\nSystem\r\nAdministrators\r\nguest"));

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_BTN_LOOKUP));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR WINAPI UserDlgProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	int nArgc = __argc;
#ifdef UNICODE
	PCTSTR* ppArgv = (PCTSTR*) CommandLineToArgvW(GetCommandLine(), &nArgc);
#else
	PCTSTR* ppArgv = (PCTSTR*) __argv;
#endif

	g_hinstExe = hinstExe;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	if(nArgc>1)
	{
		g_docycles = _tcstoul(ppArgv[1], nullptr, 0);
		if(g_docycles<=0)
			g_docycles = 1;
	}

	DlgPrivate_st dlgdata = { _T("Hello.\r\nPrivate string here.") };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, (LPARAM)&dlgdata);

	return 0;
}
