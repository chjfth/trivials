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

#define TIMER_ID 1

HKL g_curhkl;

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false)
{
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	buf[0]=_T('['); buf[1]=_T('\0'); buf[bufchars-1] = _T('\0');
	if(ymd) {
		_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%04d-%02d-%02d "), buf, 
			st.wYear, st.wMonth, st.wDay);
	}
	_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%02d:%02d:%02d.%03d]"), buf,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return buf;
}

void vaDbg(const TCHAR *fmt, ...)
{
	static int count = 0;
	static DWORD s_prev_msec = GetTickCount();

	DWORD now_msec = GetTickCount();

	TCHAR buf[1000] = {0};

	// Print timestamp to show that time has elapsed for more than one second.
	DWORD delta_msec = now_msec - s_prev_msec;
	if(delta_msec>=1000)
	{
		_tprintf(_T(".\n"));
	}

	TCHAR timebuf[40] = {};
	now_timestr(timebuf, ARRAYSIZE(timebuf));

	_sntprintf_s(buf, _TRUNCATE, _T("%s (+%3u.%03us) "), 
		timebuf, 
		delta_msec/1000, delta_msec%1000);

	int prefixlen = (int)_tcslen(buf);

	va_list args;
	va_start(args, fmt);

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);

	va_end(args);

	OutputDebugString(buf);
	OutputDebugString(_T("\n"));

	s_prev_msec = now_msec;
}

int vaMsgBox(HWND hwnd, UINT utype, const TCHAR *szTitle, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);

	TCHAR msgtext[4000] = {};
	_vsntprintf_s(msgtext, _TRUNCATE, szfmt, args);

	int ret = MessageBox(hwnd, msgtext, szTitle, utype);

	va_end(args);
	return ret;
}

void dlg_ComboboxPopulateHKLs(HWND hdlg)
{
	// Reset combobox content and start over.

	static int s_callcount = 0;

	HWND hCbx = GetDlgItem(hdlg, IDC_COMBO1);
	ComboBox_ResetContent(hCbx);

	TCHAR text[100] = {};
	HKL arHkl[100] = {};
	int nkl = GetKeyboardLayoutList(ARRAYSIZE(arHkl), arHkl);
	assert(nkl>0);

	HKL curhkl = GetKeyboardLayout(0);
	g_curhkl = curhkl;

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

			_sntprintf_s(text, _TRUNCATE, _T("Current HKL value: 0x%04X.%04X (+%d)"),
				HIWORD(hkl), langid, ++s_callcount);
			SetDlgItemText(hdlg, IDC_LABEL1, text);
		}
	}	
}

void Dlg_OnTimer(HWND hdlg, UINT id)
{
	assert(id==TIMER_ID);

	HKL curhkl = GetKeyboardLayout(0);
	if(curhkl!=g_curhkl)
	{
		dlg_ComboboxPopulateHKLs(hdlg);
	}
}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
//	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);
//	TCHAR textbuf[200];

	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		HWND hCbx = GetDlgItem(hdlg, IDC_COMBO1);

		int idx = ComboBox_GetCurSel(hCbx);
		HKL newhkl = (HKL)ComboBox_GetItemData(hCbx, idx);

		if(newhkl==g_curhkl)
			break;

		HKL oldhkl = ActivateKeyboardLayout(newhkl, 0);
		if(oldhkl==0)
		{
			vaMsgBox(hdlg, MB_OK|MB_ICONHAND, 
				_T("Something wrong"),
				_T("ActivateKeyboardLayout(0x%X) fail, winerr=%d"),
				(UINT)(UINT_PTR)newhkl, GetLastError());
		}

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

	dlg_ComboboxPopulateHKLs(hdlg);

	SetTimer(hdlg, TIMER_ID, 200, NULL);

	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
	jul->AnchorControl(0,0, 100,0, IDC_COMBO1);
	jul->AnchorControl(100,0, 100,0, IDC_BUTTON1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT1);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	
	return 0; // Let Dlg-manager respect SetFocus().
}

/* void Cls_OnInputLanguageChange(HWND hwnd, WORD charset, HKL hkl) */
#define HANDLE_WM_INPUTLANGCHANGE(hwnd, wParam, lParam, fn) \
	((fn)((hwnd), (WORD)(wParam), (HKL)(lParam)), 0L)

void Cls_OnInputLanguageChange(HWND hdlg, WORD charset, HKL hkl)
{
	vaDbg(_T("WM_INPUTLANGCHANGE: charset=%d , HKL=0x%04X.%04X"), 
		charset, HIWORD(hkl), LOWORD(hkl));
}

INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_MSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_MSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
		HANDLE_MSG(hdlg, WM_TIMER,         Dlg_OnTimer);
		HANDLE_MSG(hdlg, WM_INPUTLANGCHANGE, Cls_OnInputLanguageChange);
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
