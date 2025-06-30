#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include <crtdbg.h>
#include "resource.h"

#include "iversion.h"

#include "../utils.h"

#include <CHIMPL_vaDBG_is_vaDbgTs.h>

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#define CxxWindowSubclass_IMPL
#include <mswin/CxxWindowSubclass.h>

#define DlgTooltipEasy_IMPL
#define DlgTooltipEasy_DEBUG
#include <mswin/DlgTooltipEasy.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};


const TCHAR* my_DlgttGetUsageText(HWND hwndUic, void *userctx)
{
	static TCHAR stext[2000];

	TCHAR szWndcls[80];
	GetClassName(hwndUic, szWndcls, ARRAYSIZE(szWndcls));

	_sntprintf_s(stext, _TRUNCATE, _T("Usage tip here: {%s} hwnd=0x%X"), szWndcls, PtrToUint(hwndUic)); 

	return stext;
}

const TCHAR* my_DlgttGetContentText(HWND hwndUic, void *userctx)
{
	static TCHAR stext[2000];

	HWND hdlg = GetParent(hwndUic);
	HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	TCHAR szWndclass[80];
	GetClassName(hwndUic, szWndclass, ARRAYSIZE(szWndclass));

	if (hwndUic == hedit)
	{
		GetDlgItemText(hdlg, IDC_EDIT_LOGMSG, stext, ARRAYSIZE(stext));
		//_sntprintf_s(stext, _TRUNCATE, _T("%s"), );
	}
	else
	{
		_sntprintf_s(stext, _TRUNCATE,
			_T("Content tip here: {%s} hwnd=0x%X\r\n\r\n")
			_T("Line1\r\n\r\n")
			_T("Line2\r\n\r\n")
			_T("Line3\r\n\r\n")
			_T("Line4\r\n\r\n")
			_T("Line5\r\n\r\n")
			_T("Line6\r\n\r\n")
			_T("Line7\r\n\r\n")
			_T("Line8\r\n\r\n")
			_T("Line9\r\n\r\n")
	// 		_T("Line10\r\n\r\n")
	// 		_T("Line11\r\n\r\n")
	// 		_T("Line12\r\n\r\n")
	// 		_T("Line13\r\n\r\n")
			_T("Content end.")
			, szWndclass, PtrToUint(hwndUic));
	}

	return stext;
}


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st &prdata = *(DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	Dlgtte_err terr1, terr2;

	switch (id) 
	{{
	case IDB_AddEasyTooltip:
	{
//		vaDbgTs(_T("======== IDB_AddEasyTooltip Dump-memleaks:"));
//		_CrtDumpMemoryLeaks();

		BOOL bAutoTip = IsDlgButtonChecked(hdlg, IDCK_AutoFocusTip);

		HWND hwndEdt = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
		terr1 = Dlgtte_EnableTooltip(hwndEdt, my_DlgttGetUsageText, NULL, my_DlgttGetContentText, NULL,
			Dlgtte_BalloonUp | (bAutoTip ? Dlgtte_AutoContentTipOnFocus :Dlgtte_Flags0)
			);

		HWND hwndBtn = GetDlgItem(hdlg, IDB_AddEasyTooltip);
		terr2 = Dlgtte_EnableTooltip(hwndBtn, my_DlgttGetUsageText, NULL, 
			my_DlgttGetContentText, NULL, 
			Dlgtte_BalloonDown | (bAutoTip ? Dlgtte_AutoContentTipOnFocus : Dlgtte_Flags0)
		);

		if (terr1 || terr2) // will not happen, as soon as input HWND exists
		{
			vaMsgBox(hdlg, MB_OK|MB_ICONERROR, NULL,
				_T("ERROR occurred. Editbox err=%d , Button err=%d"), terr1, terr2);
		}

		break;
	}
	case IDB_DelEasyTooltip:
	{
		HWND hwndEdt = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
		terr1 = Dlgtte_RemoveTooltip(hwndEdt);

		HWND hwndBtn = GetDlgItem(hdlg, IDB_AddEasyTooltip);
		terr2 = Dlgtte_RemoveTooltip(hwndBtn);
		
		if (terr1 || terr2) // will not happen, as soon as input HWND exists
		{
			vaMsgBox(hdlg, MB_OK | MB_ICONERROR, NULL,
				_T("ERROR occurred. Editbox err=%d , Button err=%d"), terr1, terr2);
		}

//		vaDbgTs(_T("======== IDB_DelEasyTooltip Dump-memleaks:"));
//		_CrtDumpMemoryLeaks();

		break;
	}
	case IDCK_AutoFocusTip:
	{
		BOOL bAuto = IsDlgButtonChecked(hdlg, IDCK_AutoFocusTip);
		
		HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
		HWND hbtn = GetDlgItem(hdlg, IDB_AddEasyTooltip);

		Dlgtte_BitFlags_et flagsEdit = Dlgtte_Flags0;
		Dlgtte_BitFlags_et flagsBtn = Dlgtte_Flags0;
		
		Dlgtte_err err = Dlgtte_GetFlags(hedit, &flagsEdit);
		if (err) {
			vaMsgBox(hdlg, MB_OK | MB_ICONWARNING, NULL,
				_T("[Editbox] Dlgtte_GetFlags()=%d\n\nEasyTooltip not enabled yet."), err);
			break;
		}

		err = Dlgtte_GetFlags(hbtn, &flagsBtn);
		if (err) {
			vaMsgBox(hdlg, MB_OK | MB_ICONWARNING, NULL,
				_T("[Button] Dlgtte_GetFlags()=%d\n\nEasyTooltip not enabled yet."), err);
			break;
		}

		auto newflagsEdit = flagsEdit;
		auto newflagsBtn = flagsBtn;
		if(bAuto) {
			newflagsEdit = flagsEdit | Dlgtte_AutoContentTipOnFocus;
			newflagsBtn  = flagsBtn  | Dlgtte_AutoContentTipOnFocus;
		}
		else {
			newflagsEdit = flagsEdit & (Dlgtte_BitFlags_et)~Dlgtte_AutoContentTipOnFocus;
			newflagsBtn  = flagsBtn  & (Dlgtte_BitFlags_et)~Dlgtte_AutoContentTipOnFocus;
		}
		Dlgtte_SetFlags(hedit, newflagsEdit);
		Dlgtte_SetFlags(hbtn , newflagsBtn);

		break;
	}
	case IDB_TipShow:
	{
		HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
		Dlgtte_ShowContentTooltip(hedit, true);
		break;
	}
	case IDB_TipHide:
	{
		HWND hedit = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

		HWND hwndUsageTooltip, hwndContentTooltip;
		Dlgtte_GetTooltipHwnd(hedit, &hwndUsageTooltip, &hwndContentTooltip);
		vaDbgTs(_T("In IDB_TipHide: hwndUsageTooltip=0x%X, hwndContentTooltip=0x%X"), hwndUsageTooltip, hwndContentTooltip);

		Dlgtte_ShowContentTooltip(hedit, false);
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
	
	jul->AnchorControl(0,100, 20,100, IDB_AddEasyTooltip);
	jul->AnchorControl(20,100, 40,100, IDB_DelEasyTooltip);

	jul->AnchorControls(100,100, 100,100, IDCK_AutoFocusTip, IDB_TipShow, IDB_TipHide, -1);
}


BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	DlgPrivate_st &prdata = *(DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)&prdata);
	
	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		testDlgTooltipEasy_VMAJOR, testDlgTooltipEasy_VMINOR, testDlgTooltipEasy_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);
	
	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, prdata.mystr);

	CheckDlgButton(hdlg, IDCK_AutoFocusTip, TRUE);

	HWND hbtnStart = GetDlgItem(hdlg, IDB_AddEasyTooltip);
	Dlgtte_EnableStaticUsageTooltip(hbtnStart, 
		_T("Click this button to start.\n\n")
		_T("(Usage tooltip can be multiline.)")
	);

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

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	DlgPrivate_st dlgdata = { 
		_T("Click [Add EasyTooltip] button to start.\r\n\r\n")
		_T("Only [Add EasyTooltip] and the editbox contains tooltips. The two are called hottools.\r\n\r\n")
		_T("Text inside this editbox will become this editbox's content-tooltip.\r\n\r\n")
		_T("If [Auto focus-tip] is checked, content-tooltip will show automatically when a hottool has focus.")
	};
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, (LPARAM)&dlgdata);

	return 0;
}
