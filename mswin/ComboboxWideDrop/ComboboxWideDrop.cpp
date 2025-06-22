#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#include "utils.h"

#include <CHIMPL_vaDBG_is_vaDbgTs.h>

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#include <mswin/winuser.itc.h>

#define Combobox_EnableWideDrop_IMPL
#define Combobox_EnableWideDrop_DEBUG
#include <mswin/Combobox_EnableWideDrop.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	switch (id) 
	{{
	case IDB_AddTextToCombo:
	{
		TCHAR tbuf[4000] = {};
		GetDlgItemText(hdlg, IDE_NewText, tbuf, ARRAYSIZE(tbuf));
		HWND hcbx = GetDlgItem(hdlg, IDC_COMBO1);

		if(tbuf[0])
			ComboBox_AddString(hcbx, tbuf);
		else
		{
			const int cycles = 123000;
			
			HWND hemsg = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
			vaSetWindowText(hemsg, _T("Testing API %d cycles..."), cycles);
			UpdateWindow(hemsg);

			for(int i=0; i<cycles; i++) // mem leak test
			{
				DlgboxCbw_err e1 = Dlgbox_EnableComboboxWideDrop(hdlg);
				DlgboxCbw_err e2 = Dlgbox_DisableComboboxWideDrop(hdlg);
			}
			vaSetDlgItemText(hdlg, IDC_EDIT_LOGMSG, _T("Testing API %d cycles done."));

			vaMsgBox(hdlg, MB_OK, NULL, _T("Empty text, nothing to do."));
		}
		break;
	}
	case IDCK_EnableWideDrop:
	{
		HWND hckb = GetDlgItem(hdlg, IDCK_EnableWideDrop);
		BOOL isChk = Button_GetCheck(hckb);
		if(!isChk)
		{
			DlgboxCbw_err err = Dlgbox_EnableComboboxWideDrop(hdlg);
			if(!err || err==DlgboxCbw_AlreadyEnabled)
				Button_SetCheck(hckb, TRUE);
		}
		else
		{
			DlgboxCbw_err err = Dlgbox_DisableComboboxWideDrop(hdlg);
			if(!err || err!=DlgboxCbw_ChainMoved)
				Button_SetCheck(hckb, FALSE);
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

void init_ComboboxList(HWND hdlg)
{
	HWND hcbx = GetDlgItem(hdlg, IDC_COMBO1);
	ComboBox_AddString(hcbx, _T("ABC"));
	ComboBox_AddString(hcbx, _T("0123456789"));
	ComboBox_AddString(hcbx, _T("The Quick Brown Fox Jumps Over the Lazy Dog."));
	ComboBox_SetCurSel(hcbx, 0);

	HWND hedit = GetDlgItem(hdlg, IDE_NewText);
	Edit_SetCueBannerText(hedit, _T("Type a long sentence and add it to the combobox."));

	TCHAR longtext[] = _T("01234567890 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz");
	const int textlen = ARRAYSIZE(longtext)-1;

	hcbx = GetDlgItem(hdlg, IDC_COMBO2);
	for(int i=0; i<textlen; i++)
	{
		TCHAR c = longtext[i+1];
		longtext[i+1] = '\0';

		ComboBox_AddString(hcbx, longtext);

		longtext[i+1] = c;
	}
	ComboBox_SetCurSel(hcbx, textlen-1); // select final item
}


static void Dlg_EnableJULayout(HWND hdlg)
{
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_COMBO1);
	jul->AnchorControl(0,0, 100,0, IDE_NewText);
	jul->AnchorControl(100,0, 100,0, IDB_AddTextToCombo);

	jul->AnchorControl(0,0, 100,0, IDC_COMBO2);

	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
}

BOOL Dlg_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	vaSetWindowText(hdlg, _T("ComboboxWideDrop v%d.%d.%d"), 
		ComboboxWideDrop_VMAJOR, ComboboxWideDrop_VMINOR, ComboboxWideDrop_VPATCH);
	
	SetDlgItemText(hdlg, IDC_EDIT_LOGMSG, 
		_T("With the help of Dlgbox_EnableComboboxWideDrop(), combobox dropdown adjusts it width automatically according to item text length within."));

	Dlg_EnableJULayout(hdlg);

	init_ComboboxList(hdlg);
	
	SNDMSG(GetDlgItem(hdlg, IDCK_EnableWideDrop), BM_CLICK, 0, 0);

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

	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, UserDlgProc, NULL);

	return 0;
}
