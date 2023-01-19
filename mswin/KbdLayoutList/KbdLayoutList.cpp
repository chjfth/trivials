#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include "resource.h"
#include "iversion.h"

#define JULAYOUT_IMPL
#include "JULayout2.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

void dlg_ComboboxPopulateHKLs(HWND hdlg)
{
	HWND hCbx = GetDlgItem(hdlg, IDC_COMBO1);
	ComboBox_ResetContent(hCbx);

	TCHAR text[100] = {};
	HKL arHkl[100] = {};
	int nkl = GetKeyboardLayoutList(ARRAYSIZE(arHkl), arHkl);
	assert(nkl>0);

	HKL curhkl = GetKeyboardLayout(0);

	int i;
	for(i=0; i<nkl; i++)
	{
		HKL hkl = arHkl[i];
		LANGID langid = LOWORD(hkl);

		TCHAR langtag[LOCALE_NAME_MAX_LENGTH+1] = {};
		LCIDToLocaleName(langid, langtag, LOCALE_NAME_MAX_LENGTH, 0);

		TCHAR langname[80] = {};
		GetLocaleInfo(langid, LOCALE_SENGLISHDISPLAYNAME, langname, ARRAYSIZE(langname));

		_sntprintf_s(text,  _TRUNCATE, 
			_T("%s[#%d] 0x%04X.%04X : %s, %s\n"),
			hkl==curhkl ? _T("* ") : _T("  ") ,
			i,
			HIWORD(hkl), langid, 
			langtag, langname
			);

		// Associate HKL value to the listbox item.
		//
		int idx = ComboBox_AddString(hCbx, text);
		ComboBox_SetItemData(hCbx, idx, hkl);

		if(hkl==curhkl)
		{
			ComboBox_SetCurSel(hCbx, idx);

			_sntprintf_s(text, _TRUNCATE, _T("Current HKL value: 0x%04X.%04X"),
				HIWORD(hkl), langid);
			SetDlgItemText(hdlg, IDC_LABEL1, text);
		}
	}	


}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	TCHAR textbuf[200];

	switch (id) 
	{{
	case IDC_BUTTON1:
	{
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

// Sets the dialog box icons
inline void chSETDLGICONS(HWND hwnd, int idi) {
	SendMessage(hwnd, WM_SETICON, TRUE,  (LPARAM)
		LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		MAKEINTRESOURCE(idi)));
	SendMessage(hwnd, WM_SETICON, FALSE, (LPARAM)
		LoadIcon((HINSTANCE) GetWindowLongPtr(hwnd, GWLP_HINSTANCE),
		MAKEINTRESOURCE(idi)));
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)prdata);
	
	chSETDLGICONS(hdlg, IDI_WINMAIN);

/*
	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		KbdLayoutList_VMAJOR, KbdLayoutList_VMINOR, KbdLayoutList_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);
	
	SetDlgItemText(hdlg, IDC_EDIT1, prdata->mystr);
*/

	dlg_ComboboxPopulateHKLs(hdlg);

	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
	jul->AnchorControl(0,0, 100,0, IDC_COMBO1);
	jul->AnchorControl(100,0, 100,0, IDC_BUTTON1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT1);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	
	return 0; // Let Dlg-manager respect SetFocus().
}

INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_MSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_MSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) 
{
	g_hinstExe = hinstExe;

	DlgPrivate_st dlgdata = { _T("Hello.\r\nPrivate string here.") };
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)&dlgdata);

	return 0;
}
