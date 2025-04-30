#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <NTSecAPI.h>
#include <AclAPI.h>
#include <sddl.h>
#include <tchar.h>
#include <stdio.h>
#include <limits.h>
#include "resource.h"

#include "iversion.h"

#include "utils.h"

#include <StringHelper.h>

#include <EnsureClnup_mswin.h>

#include <mswin/winnt.itc.h>
#include <mswin/WinError.itc.h>
using namespace itc;

#include <JAutoBuf.h>
#include <mswin/JULayout2.h>

#include "helper.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};


//typedef TScalableArray<TCHAR> TSA_TCHARs;
//typedef TScalableArray<UCHAR> TSA_bytes;

struct Trustee_st
{
	JAutoBuf<TCHAR> abName;  // user-name or group-name
	JAutoBuf<SID, 1> abSid; // SID storage
};

MakeDelega_CleanupCxxPtrN(Trustee_st, CecArray_Trustee)

struct Trustees_st
{
	int count;
	CecArray_Trustee ecaTrustees;

/*
	Trustees_st()
	{
		this->count = 0;
		vaDbgTs(_T("Trustees_st ctor, this=%p"), this);
	}
	~Trustees_st()
	{
		vaDbgTs(_T("Trustees_st dtor, this=%p"), this);
	}
*/
};


void appendmsg(HWND hdlg, const TCHAR *fmt, ...)
{
	HWND hemsg = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	TCHAR tbuf[400];
	va_list args;
	va_start(args, fmt);

	_vsntprintf_s(tbuf, _TRUNCATE, fmt, args);
	vaAppendText_mled(hemsg, _T("%s\r\n"), tbuf);
	
	UpdateWindow(hemsg);

	va_end(args);
}

Trustees_st myCreateTrusteeArray(const TCHAR *pszTrusteelist)
{
	// pszTrusteeList, trustees separated by semicolons

	BOOL succ = 0;
	DWORD winerr = 0;

	Trustees_st tees = { 0 };

	// Count input trustees

	StringSplitter<decltype(pszTrusteelist), IsSemicolon, StringSplitter_TrimSpacechar> 
		sp(pszTrusteelist);
	for(;; tees.count++)
	{
		int len = 0;
		int pos = sp.next(&len);
		if(pos==-1)
			break;
	}

	if(tees.count==0)
		return tees;

	Trustee_st *arTrustee = new Trustee_st[tees.count];
	tees.ecaTrustees = arTrustee;

	sp.reset();
	int i = 0;
	for(;; i++)
	{
		int len = 0;
		int pos = sp.next(&len);
		if(pos==-1)
			break;

		Trustee_st &tee = arTrustee[i];
		
		// fill tee.abName

		tee.abName = len+1;
		_tcsncpy_s(tee.abName, tee.abName, &pszTrusteelist[pos], len);

		// fill tee.abSid
		AutoTCHARs abDomName;
		SID_NAME_USE sidtype = SidTypeInvalid;
		do {
			succ = LookupAccountName(NULL, tee.abName,
				tee.abSid, tee.abSid, 
				abDomName, abDomName, // no-use but API want it
				&sidtype);
		} while(!succ && IsBufferSmall(winerr=GetLastError()));

		if (!succ) {
			THROWE(_T("LookupAccountName() fail for trustee \"%s\", winerr=%s"),
				tee.abName.Bufptr(), ITCSv(winerr, WinError));
		}
	}

	return tees;
}


#define WINAPI_CHECK_THROWE(is_succ, apiname) \
	if(!is_succ) { \
		THROWE(_T("%s() fail, winerr=%s"), _T(#apiname), ITCS_WinError); \
	}



int cal_AceSizeTotal(const Trustees_st &tees)
{
	int totalbytes = 0;
	const int bytesAceHeaderMask = sizeof(ACE_HEADER) + sizeof(ACCESS_MASK);

	for(int i=0; i<tees.count; i++)
	{
		const Trustee_st &tee = tees.ecaTrustees[i];
		int sidbytes = tee.abSid.Bytes();
		totalbytes += (bytesAceHeaderMask + sidbytes);
	}

	return totalbytes;
}

void fill_Dacl_with_Aces(ACL *pAcl, const Trustees_st &tees, 
	DWORD aceflags, ACCESS_MASK AceMask,
	decltype(AddAccessAllowedAceEx) procAddAce)
{
	DWORD aclRevision = ACL_REVISION;

	for (int i = 0; i < tees.count; i++)
	{
		const Trustee_st &tee = tees.ecaTrustees[i];

		BOOL succ = procAddAce(pAcl, aclRevision, 
			aceflags, AceMask,
			const_cast<SID*>(tee.abSid.Bufptr())
			);
		WINAPI_CHECK_THROWE(succ, AddAccessAllowedAceEx/AddAccessDeniedAceEx);
	}
}

void do_ShowDbgSetNamedSecurityInfo(HWND hdlg, 
	const Trustees_st& teesDeny, const Trustees_st& teesAllow, 
	SECURITY_INFORMATION aclWhat)
{
	int totalAces = teesDeny.count + teesAllow.count;

	if (teesDeny.count + teesAllow.count > 0)
	{
		appendmsg(hdlg, _T("%s"), _T("Will set these ACEs:"));

		int i;
		for (i = 0; i < teesDeny.count; i++)
		{
			const Trustee_st &tee = teesDeny.ecaTrustees[i];

			TCHAR *sidstr = NULL;
			ConvertSidToStringSid((PSID)tee.abSid.Ptr(), &sidstr);
			CEC_LocalFree cec = sidstr;

			appendmsg(hdlg,
				_T("[%d/%d] Deny: \"%s\", SID=%s")
				,
				i + 1, totalAces,
				tee.abName.Ptr(), sidstr);
		}

		for (i = 0; i < teesAllow.count; i++)
		{
			const Trustee_st &tee = teesAllow.ecaTrustees[i];

			TCHAR *sidstr = NULL;
			ConvertSidToStringSid((PSID)tee.abSid.Ptr(), &sidstr);
			CEC_LocalFree cec = sidstr;

			appendmsg(hdlg,
				_T("[%d/%d] Allow: \"%s\", SID=%s")
				,
				teesDeny.count + i + 1, totalAces,
				tee.abName.Ptr(), sidstr);
		}
	}

	appendmsg(hdlg, _T("aclWhat=%s"), ITCSv(aclWhat, xxx_SECURITY_INFORMATION));
}

DWORD get_AceflagsDeny(HWND hdlg)
{
	DWORD aceflags = 0;
	aceflags |= IsDlgButtonChecked(hdlg, IDCK_Deny_OBJECT_INHERIT_ACE) ? OBJECT_INHERIT_ACE : 0;
	aceflags |= IsDlgButtonChecked(hdlg, IDCK_Deny_CONTAINER_INHERIT_ACE) ? CONTAINER_INHERIT_ACE : 0;
	aceflags |= IsDlgButtonChecked(hdlg, IDCK_Deny_NO_PROPAGATE_INHERIT_ACE) ? NO_PROPAGATE_INHERIT_ACE : 0;
	aceflags |= IsDlgButtonChecked(hdlg, IDCK_Deny_INHERIT_ONLY_ACE) ? INHERIT_ONLY_ACE : 0;
	return aceflags;
}

DWORD get_AceflagsAllow(HWND hdlg)
{
	DWORD aceflags = 0;
	aceflags |= IsDlgButtonChecked(hdlg, IDCK_Allow_OBJECT_INHERIT_ACE) ? OBJECT_INHERIT_ACE : 0;
	aceflags |= IsDlgButtonChecked(hdlg, IDCK_Allow_CONTAINER_INHERIT_ACE) ? CONTAINER_INHERIT_ACE : 0;
	aceflags |= IsDlgButtonChecked(hdlg, IDCK_Allow_NO_PROPAGATE_INHERIT_ACE) ? NO_PROPAGATE_INHERIT_ACE : 0;
	aceflags |= IsDlgButtonChecked(hdlg, IDCK_Allow_INHERIT_ONLY_ACE) ? INHERIT_ONLY_ACE : 0;
	return aceflags;
}

ACCESS_MASK get_AceMask(HWND hdlg)
{
	TCHAR tbuf[20] = {};
	TCHAR *endptr = nullptr;
	GetDlgItemText(hdlg, IDE_CustomAccessMask, tbuf, ARRAYSIZE(tbuf));
	ULONG val = _tcstoul(tbuf, &endptr, 0);
	if (*endptr != '\0')
		THROWE(_T("%s"), _T("Custom Access Mask invalid input format."));
	else if (val == ULONG_MAX)
		THROWE(_T("%s"), _T("Custom Access Mask value too large."));
	else if (val == 0)
		THROWE(_T("%s"), _T("Custom Access Mask input is invalid, should not be 0."));

	return val;
}

void ui_UpdateAccessMask(HWND hdlg, UINT uic)
{
	TCHAR tbuf[20] = {};
	if (uic == IDWB_FILE_ALL_ACCESS)
	{
		_sntprintf_s(tbuf, _TRUNCATE, _T("0x%08X"), FILE_ALL_ACCESS);
		SetDlgItemText(hdlg, IDE_CustomAccessMask, tbuf);
	}
	else if (uic == IDWB_GENERIC_ALL)
	{
		_sntprintf_s(tbuf, _TRUNCATE, _T("0x%08X"), GENERIC_ALL);
		SetDlgItemText(hdlg, IDE_CustomAccessMask, tbuf);
	}
	else if (uic == IDWB_CustomAccessMask)
	{
		SetFocus(GetDlgItem(hdlg, IDE_CustomAccessMask));
	}
}


void do_SetNamedSecurityInfo(HWND hdlg)
{
	//// Grab user input from UI and finally call SetNamedSecurityInfo for these UI input.

	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, _T(""));

	TCHAR ntfspath[MAX_PATH] = {};
	GetDlgItemText(hdlg, IDE_NtfsPath, ntfspath, ARRAYSIZE(ntfspath));

	//
	// Get deny-trustee list and allow-trustee list from UI.
	//

	BOOL succ = 0;
	TCHAR tbuf[400] = {};
	DWORD aclRevision = ACL_REVISION;

	GetDlgItemText(hdlg, IDE_DenyTrustees, tbuf, ARRAYSIZE(tbuf));
	Trustees_st teesDeny = myCreateTrusteeArray(tbuf);

	GetDlgItemText(hdlg, IDE_AllowTrustees, tbuf, ARRAYSIZE(tbuf));
	Trustees_st teesAllow = myCreateTrusteeArray(tbuf);

	if (teesDeny.count == 0 && teesAllow.count == 0)
	{
		int ret = vaMsgBox(hdlg, MB_OKCANCEL, NULL, _T("%s"),
			_T("You do not provide any DENY-trustees or ALLOW-trustees, so you are setting an Empty-DACL.\n")
			_T("So no one has permission to access this NTFS path .\n")
			_T("\n")
			_T("Are you sure?"));
		if (ret == IDCANCEL)
			return;
	}

	// Calculate ACL size for InitializeAcl() , (Ref: 20221125.i1)
	int AclBytesTotal = sizeof(ACL);
	AclBytesTotal += cal_AceSizeTotal(teesDeny);
	AclBytesTotal += cal_AceSizeTotal(teesAllow);
	
	// Allocate ACL buffer and call InitializeAcl.
	ACL *pDacl = (ACL*)new UCHAR[AclBytesTotal];
	CEC_raw_delete cec_aclbuf = pDacl;

	succ = InitializeAcl(pDacl, AclBytesTotal, aclRevision);
	WINAPI_CHECK_THROWE(succ, InitializeAcl);

	DWORD aceflagsDeny = get_AceflagsDeny(hdlg);
	DWORD aceflagsAllow = get_AceflagsAllow(hdlg);

	ACCESS_MASK AceMask = get_AceMask(hdlg);

	appendmsg(hdlg, _T("Deny-ACE flags: 0x%08X , %s"), aceflagsDeny, ITCSv(aceflagsDeny, AceFlags));
	appendmsg(hdlg, _T("Allow-ACE flags: 0x%08X , %s"), aceflagsAllow, ITCSv(aceflagsAllow, AceFlags));
	
	DWORD fileattr = GetFileAttributes(ntfspath);
	if (fileattr == INVALID_FILE_ATTRIBUTES)
	{
		appendmsg(hdlg, _T("Access Mask to use(on both Deny/Allow): 0x%08X (not knowing whether it is a file or directory)"));
	}
	else
	{
		int isDir = fileattr & FILE_ATTRIBUTE_DIRECTORY;
		appendmsg(hdlg, _T("Access Mask to use(on both Deny/Allow): 0x%08X , %s"),
			AceMask, ITCSv(AceMask, isDir?DirectoryRights:FileRights));
	}

	fill_Dacl_with_Aces(pDacl, teesDeny, aceflagsDeny, AceMask, AddAccessDeniedAceEx);
	fill_Dacl_with_Aces(pDacl, teesAllow, aceflagsAllow, AceMask, AddAccessAllowedAceEx);

	//
	// Start calling SetNamedSecurityInfo()
	//

	DWORD aclWhat = DACL_SECURITY_INFORMATION;
	
	int isProtectedDacl = Button_GetCheck(GetDlgItem(hdlg, IDCK_ProtectedDacl));
	if (isProtectedDacl)
		aclWhat |= PROTECTED_DACL_SECURITY_INFORMATION;

	int isUnprotectedDacl = Button_GetCheck(GetDlgItem(hdlg, IDCK_UnProtectedDacl));
	if (isUnprotectedDacl)
		aclWhat |= UNPROTECTED_DACL_SECURITY_INFORMATION;

	do_ShowDbgSetNamedSecurityInfo(hdlg, teesDeny, teesAllow, aclWhat);

	TCHAR timestr[100] = {};
	appendmsg(hdlg, _T("%s Start calling SetNamedSecurityInfo()"),
		now_timestr(timestr, ARRAYSIZE(timestr)));

	DWORD msec_start = TrueGetMillisec();
	DWORD winerr = SetNamedSecurityInfo(
		ntfspath,            // object name
		SE_FILE_OBJECT,      // object type
		aclWhat,
		NULL,                // owner info (unchanged)
		NULL,                // group info (unchanged)
		pDacl, // explicit DACL to set (may be a copy of the current one)
		NULL                 // SACL info (unchanged)
	);
	DWORD msec_used = TrueGetMillisec() - msec_start;

	appendmsg(hdlg, _T("%s Done calling SetNamedSecurityInfo()"),
		now_timestr(timestr, ARRAYSIZE(timestr)));

	appendmsg(hdlg, _T("Time cost: %u millisec"), msec_used);

	if (winerr) 
	{
		THROWE(_T("SetNamedSecurityInfo() fail, winerr=%s"), ITCSv(winerr, WinError)); 
	}
}



void DO_SetNamedSecurityInfo(HWND hdlg)
{
	try
	{
		do_SetNamedSecurityInfo(hdlg);
	}
	catch (const ErrMsg& e)
	{
		TCHAR timestr[40] = _T("");
		now_timestr(timestr, ARRAYSIZE(timestr));

		appendmsg(hdlg, _T("%s[ERROR] In %s():\r\n%s"), timestr, e.m_func, e.m_errmsg);

		vaMsgBox(hdlg, MB_OK|MB_ICONERROR, NULL, 
			_T("[ERROR] In %s():\r\n%s"), e.m_func, e.m_errmsg);
	}
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
//	TCHAR textbuf[200];

	switch (id) 
	{{
	case IDB_SetSecurityInfo:
	{
		DO_SetNamedSecurityInfo(hdlg);

		break;
	}
	case IDWB_FILE_ALL_ACCESS:
	case IDWB_GENERIC_ALL:
	case IDWB_CustomAccessMask:
		ui_UpdateAccessMask(hdlg, id);
		break;

	case IDE_CustomAccessMask:
	{
		if (LOWORD(codeNotify) == EN_SETFOCUS)
			CheckRadioButton(hdlg, IDWB_FILE_ALL_ACCESS, IDWB_CustomAccessMask, IDWB_CustomAccessMask);
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
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDE_NtfsPath);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
	jul->AnchorControl(0,100, 0,100, IDB_SetSecurityInfo);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	vaSetWindowText(hdlg, _T("ntfsSetNamedSecurityInfo1 v%d.%d.%d"), 
		ntfsSetNamedSecurityInfo1_VMAJOR, ntfsSetNamedSecurityInfo1_VMINOR, ntfsSetNamedSecurityInfo1_VPATCH);
	
	// SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, prdata->mystr);
	SetDlgItemText(hdlg, IDE_NtfsPath, _T("d:\\Atest"));

	const TCHAR *cuetext = _T("Users or groups, separated by semicolons.");
	Edit_SetCueBannerText(GetDlgItem(hdlg, IDE_DenyTrustees), cuetext);
	Edit_SetCueBannerText(GetDlgItem(hdlg, IDE_AllowTrustees), cuetext);

	CheckDlgButton(hdlg, IDCK_Deny_OBJECT_INHERIT_ACE, 1);
	CheckDlgButton(hdlg, IDCK_Deny_CONTAINER_INHERIT_ACE, 1);
	CheckDlgButton(hdlg, IDCK_Allow_OBJECT_INHERIT_ACE, 1);
	CheckDlgButton(hdlg, IDCK_Allow_CONTAINER_INHERIT_ACE, 1);

	CheckRadioButton(hdlg, IDWB_FILE_ALL_ACCESS, IDWB_CustomAccessMask, IDWB_FILE_ALL_ACCESS);
	ui_UpdateAccessMask(hdlg, IDWB_FILE_ALL_ACCESS);

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDE_NtfsPath));
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
	g_hinstExe = hinstExe;

	const char *pdate =  __func__;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	DlgPrivate_st dlgdata = { _T("Hello.\r\nPrivate string here.") };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, (LPARAM)&dlgdata);

	return 0;
}
