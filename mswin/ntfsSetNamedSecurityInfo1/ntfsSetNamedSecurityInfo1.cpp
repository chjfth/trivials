#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <NTSecAPI.h>
#include <AclAPI.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "utils.h"

#include <StringHelper.h>

#include <EnsureClnup_mswin.h>

#include <mswin/WinError.itc.h>
using namespace itc;

#define JAUTOBUF_IMPL
#define vaDBG vaDbgTs
#include <JAutoBuf.h>

#define JULAYOUT_IMPL
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

Trustees_st myAllocTrusteeArray(const TCHAR *pszTrusteelist)
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

void fill_Dacl_with_Aces(ACL *pAcl, const Trustees_st &tees, DWORD aceflags,
	decltype(AddAccessAllowedAceEx) procAddAce)
{
	DWORD aclRevision = ACL_REVISION;

	for (int i = 0; i < tees.count; i++)
	{
		const Trustee_st &tee = tees.ecaTrustees[i];

		BOOL succ = procAddAce(pAcl, aclRevision, aceflags,
			GENERIC_ALL | STANDARD_RIGHTS_ALL,
			const_cast<SID*>(tee.abSid.Bufptr())
			);
		WINAPI_CHECK_THROWE(succ, AddAccessAllowedAceEx/AddAccessDeniedAceEx);
	}
}

void do_SetNamedSecurityInfo(HWND hdlg)
{
	//// Grab user input from UI and finally call SetNamedSecurityInfo for these UI input.

	// Get deny-trustee list and allow-trustee list from UI.

	BOOL succ = 0;
	TCHAR tbuf[400] = {};
	DWORD aclRevision = ACL_REVISION;
	DWORD aceflags = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE;

	GetDlgItemText(hdlg, IDE_DenyTrustees, tbuf, ARRAYSIZE(tbuf));
	Trustees_st teesDeny = myAllocTrusteeArray(tbuf);

	GetDlgItemText(hdlg, IDE_AllowTrustees, tbuf, ARRAYSIZE(tbuf));
	Trustees_st teesAllow = myAllocTrusteeArray(tbuf);

	if (teesDeny.count == 0 && teesAllow.count == 0)
	{
		int ret = vaMsgBox(hdlg, MB_OKCANCEL, NULL, _T("%s"),
			_T("You do not provide any trustee in deny/allow list, so you are setting an Empty-DACL.\n")
			_T("So no one has permission to access this NTFS path .\n")
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

	fill_Dacl_with_Aces(pDacl, teesDeny, aceflags, AddAccessDeniedAceEx);
	fill_Dacl_with_Aces(pDacl, teesAllow, aceflags, AddAccessAllowedAceEx);

	//
	// Start calling SetNamedSecurityInfo()
	//

	TCHAR timestr[100] = {};
	appendmsg(hdlg, _T("%s Start calling SetNamedSecurityInfo()"),
		now_timestr(timestr, ARRAYSIZE(timestr)));

	DWORD whattoset = DACL_SECURITY_INFORMATION; // | PROTECTED_DACL_SECURITY_INFORMATION;
	
//	HWND hemsg = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
	
	TCHAR ntfspath[MAX_PATH] = {};
	GetDlgItemText(hdlg, IDE_NtfsPath, ntfspath, ARRAYSIZE(ntfspath));
	
	DWORD msec_start = TrueGetMillisec();
	DWORD winerr = SetNamedSecurityInfo(
		ntfspath,            // object name
		SE_FILE_OBJECT,      // object type
		whattoset,
		NULL,                // owner info (unchanged)
		NULL,                // group info (unchanged)
		pDacl, // explicit DACL to set (may be a copy of the current one)
		NULL                 // SACL info (unchanged)
	);
	DWORD msec_used = TrueGetMillisec() - msec_start;

	appendmsg(hdlg, _T("%s Done calling SetNamedSecurityInfo()"),
		now_timestr(timestr, ARRAYSIZE(timestr)));

	appendmsg(hdlg, _T("Time cost: %u millisec"), msec_used);

	WINAPI_CHECK_THROWE(winerr?FALSE:TRUE, SetNamedSecurityInfo);
}



void test1(HWND hdlg)
{
	try
	{
		do_SetNamedSecurityInfo(hdlg);
	}
	catch (const ErrMsg& e)
	{
		appendmsg(hdlg, _T("[ERROR] In %s:\r\n%s"), e.m_func, e.m_errmsg);
	}
}


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
//	TCHAR textbuf[200];

	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		test1(hdlg);

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

	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
	jul->AnchorControl(50,100, 50,100, IDC_BUTTON1);

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

	Dlg_EnableJULayout(hdlg);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
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
