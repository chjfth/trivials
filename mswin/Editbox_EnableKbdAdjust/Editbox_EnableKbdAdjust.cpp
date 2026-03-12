#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#define CHHI_ALL_IMPL
#include <mswin/JULayout2.h>

#include <vaDbgTs.h>
#include <mswin/utils_env.h>
#include <mswin/utils_wingui.h>

#include <mswin/Editbox_EnableKbdAdjustIntnum.h>
#include <mswin/Editbox_EnableKbdAdjustFloatnum.h>


#include "CxxDialog.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


HINSTANCE g_hinstExe;

class MainDialog : public CxxDialog
{
public:
	MainDialog(LPCTSTR mystr);

	virtual INT_PTR DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam);

private:
	const TCHAR *m_mystr;
	int m_clicks;
};

MainDialog::MainDialog(LPCTSTR mystr)
{
	m_mystr = mystr;
	m_clicks = 0;
}

void MainDialog::OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
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

static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg, 0, 0, 1000, 0);

	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
	jul->AnchorControl(50,100, 50,100, IDOK);

	jul->AnchorControls(0,0, 100,0, IDS_Intnum1, IDS_Intnum2, IDS_Floatnum1, -1);

	// If you add more controls(IDC_xxx) to the dialog, adjust them here.
}

void Do_InitEditboxKbd(HWND hdlg)
{
	EditboxKAN_err err_int = EditboxKAN_Succ;
	EditboxKAF_err err_flt = EditboxKAF_Succ;
	HWND hedit = NULL;

	// Intnum1

	hedit = GetDlgItem(hdlg, IDE_Intnum1);
	SetWindowText(hedit, _T("3"));

	int min_int1 = -100, max_int1 = 100, step_int1 = 1;
	bool is_wrap_around1 = false;
	err_int = Editbox_EnableKbdAdjustIntnum(hedit, 
		min_int1, max_int1, step_int1, is_wrap_around1, 0);
	assert(!err_int);
	vaSetDlgItemText(hdlg, IDS_Intnum1, 
		_T("Range: %d ~ %d, step %d , no wrap-around"),
		min_int1, max_int1, step_int1);

	// Intnum2

	hedit = GetDlgItem(hdlg, IDE_Intnum2);
	SetWindowText(hedit, _T("00:03"));

	int min_int2 = 0, max_int2 = 59, step_int2 = 1;
	bool is_wrap_around2 = true;
	const unsigned int zero_pad_width = 2; // clock minute seconds should display 00, 01, 02 etc
	err_int = Editbox_EnableKbdAdjustIntnum(hedit, 
		min_int2, max_int2, step_int2, is_wrap_around2, 
		zero_pad_width);
	assert(!err_int);
	vaSetDlgItemText(hdlg, IDS_Intnum2, 
		_T("Range: %d ~ %d, zero-padded, wrap-around"),
		min_int2, max_int2, step_int2);

	// Floatnum1

	hedit = GetDlgItem(hdlg, IDE_Floatnum1);
	SetWindowText(hedit, _T("-1.0000"));

	double min_flt1 = -3.1415926, max_flt1 = -min_flt1, step_flt1 = 0.1;
	bool is_wrap_around3 = false;
	err_flt = Editbox_EnableKbdAdjustFloatnum(hedit,
		min_flt1, max_flt1, step_flt1, _T("%.4f"), is_wrap_around3, 
		_T("Please input range from -PI to +PI.\n"));

	vaSetDlgItemText(hdlg, IDS_Floatnum1, 
		_T("Range: %.4f ~ %.4f, step: %g, no wrap-around"),
		min_flt1, max_flt1, step_flt1);

}

BOOL MainDialog::OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	vaSetWindowText(hdlg, _T("Editbox_EnableKbdAdjust demo version: %d.%d.%d"), 
		Editbox_EnableKbdAdjust_VMAJOR, Editbox_EnableKbdAdjust_VMINOR, Editbox_EnableKbdAdjust_VPATCH);
	
	Dlg_EnableJULayout(hdlg);

	Do_InitEditboxKbd(hdlg);

	SetFocus(GetDlgItem(hdlg, IDE_Intnum1));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR MainDialog::DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       OnCommand);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	MainDialog dlg(_T("Hello.\r\nPrivate string here."));
	dlg.DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL);

	return 0;
}
