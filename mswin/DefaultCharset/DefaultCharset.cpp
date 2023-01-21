#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include "resource.h"
#include "iversion.h"

#define JULAYOUT_IMPL
#include "JULayout2.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;


void mleAppendText(HWND hedit, const TCHAR *szfmt, ...)
{
	TCHAR tbuf[4000] = {};
	va_list args;
	va_start(args, szfmt);

	_vsntprintf_s(tbuf, _TRUNCATE, szfmt, args);

	int pos = GetWindowTextLength (hedit);
	
	Edit_SetSel(hedit, pos, pos);
	Edit_ReplaceSel(hedit, tbuf);

	va_end(args);
}


BYTE get_Charset_from_current_HKL()
{
	// Purpose: On program startup, we do not see WM_INPUTLANGCHANGE, 
	// until human user initiates Input-language change.
	// Before WM_INPUTLANGCHANGE, we use this function to know Charset 
	// value associated with current Input-language. 

	BOOL succ = 0;
	HKL curhkl = GetKeyboardLayout(0);

	HKL arHkl[100] = {};
	int nkl = GetKeyboardLayoutList(ARRAYSIZE(arHkl), arHkl);
	assert(nkl>0);

//	if(nkl==1)
	{
		HDC hdc = GetDC(NULL);
		SelectObject (hdc, CreateFont (0, 0, 0, 0, 0, 0, 0, 0,
			DEFAULT_CHARSET, // this corresponds to system-locale ANSI-codepage
			0, 0, 0, FIXED_PITCH, NULL)) ; 

		TEXTMETRIC tm = {};
		succ = GetTextMetrics (hdc, &tm) ;
		assert(succ);
		assert(tm.tmCharSet!=DEFAULT_CHARSET);

		ReleaseDC(NULL, hdc);

		return tm.tmCharSet;
	}

	int i;
	for(i=0; i<nkl; i++)
	{
		HKL hkl = arHkl[i];
		LANGID langid = LOWORD(hkl);

		TCHAR langtag[LOCALE_NAME_MAX_LENGTH+1] = {};
		//		LCIDToLocaleName(langid, langtag, LOCALE_NAME_MAX_LENGTH, 0);

		TCHAR langname[80] = {};
		GetLocaleInfo(langid, LOCALE_SENGLISHDISPLAYNAME, langname, ARRAYSIZE(langname));

		_tprintf(_T("%c[#%-2d] 0x%04X.%04X : %s, %s\n"),
			hkl==curhkl ? '*' : ' ',
			i,
			HIWORD(hkl), langid, 
			langtag, langname
			);
	}
}


void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	switch (id) 
	{{
	case IDC_BUTTON1:
	{
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
	chSETDLGICONS(hdlg, IDI_WINMAIN);

	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		DefaultCharset_VMAJOR, DefaultCharset_VMINOR, DefaultCharset_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);

	BYTE charset = get_Charset_from_current_HKL();

	HWND hedt = GetDlgItem(hdlg, IDC_EDIT1);

	mleAppendText(hedt, _T("get_Charset_from_current_HKL()=%d"), charset);

	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT1);
	jul->AnchorControl(50,100, 50,100, IDC_BUTTON1);

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

	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, NULL);

	return 0;
}
