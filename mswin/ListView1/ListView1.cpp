#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"
#include "iversion.h"

#include "utils.h"

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinstExe;

struct DlgPrivate_st
{
	const TCHAR *mystr;
	int clicks;
};

#undef HANDLE_WM_NOTIFY // redefine the crappy one from commctrl.h
#define HANDLE_WM_NOTIFY(hwnd, wParam, lParam, fn) \
	(  (fn)((hwnd), (int)(wParam), (NMHDR *)(lParam)),   0L  )



// Sample snippet from MSDN 2008

#define C_COLUMNS 3

// This code example adds three items, each with three subitems, to 
// a list-view control.

// hWndListView - handle to the list-view control.
// PETINFO - an application-specific structure.

typedef struct tagPETINFO
{
	TCHAR szKind[10];
	TCHAR szBreed[50];
	TCHAR szPrice[20];
}PETINFO;

// A PETINFO variable is declared and initialized as follows:

PETINFO rgPetInfo[ ] = 
{
	{TEXT("Dog"), TEXT("Poodle"), TEXT("$300.00")},
	{TEXT("Cat"), TEXT("Siamese"), TEXT("$100.00")},
	{TEXT("Fish"), TEXT("Angel Fish"), TEXT("$10.00")},
};

BOOL InitListviewColumns(HWND hWndListView) 
{ 
	TCHAR szText[256] = {};     // temporary buffer 
	LVCOLUMN lvc = {}; 
	int iCol = 0; 

	// Initialize the LVCOLUMN structure.
	// The mask specifies that the format, width, text, and subitem members
	// of the structure are valid. 
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 

	// Add the columns
	for (iCol = 0; iCol < C_COLUMNS; iCol++) 
	{ 
		lvc.iSubItem = iCol;
		lvc.pszText = szText;	
		lvc.cx = 100;     // width of column in pixels

		if ( iCol < 2 )
			lvc.fmt = LVCFMT_LEFT;  // left-aligned column
		else
			lvc.fmt = LVCFMT_RIGHT; // right-aligned column		                         

		LoadString(GetModuleHandle(NULL), 
			IDS_FIRSTCOLUMN + iCol, 
			szText, 
			sizeof(szText)/sizeof(szText[0]));

		if (ListView_InsertColumn(hWndListView, iCol, &lvc) == -1) 
			return FALSE; 
	} 
	return TRUE; 
} 

void InsertListviewItems(HWND hWndListView)
{
	// Initialize LVITEM members that are common to all items.
	LVITEM lvI = {};
	lvI.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE; 
	lvI.state = 0; 
	lvI.stateMask = 0; 

	// Initialize LVITEM members that are different for each item. 
	int index;
	for (index = 0; index < 3; index++)
	{
		lvI.iItem = index;
		lvI.iImage = index;
		lvI.iSubItem = 0;
		lvI.lParam = (LPARAM) &rgPetInfo[index];
		lvI.pszText = LPSTR_TEXTCALLBACK; // sends an LVN_GETDISP message.
		auto ret = ListView_InsertItem(hWndListView, &lvI);
		assert(ret!=-1);
	}

}

void Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	DlgPrivate_st *prdata = (DlgPrivate_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	switch (id) 
	{{
	case IDC_BUTTON1:
	{
		++(prdata->clicks);

		HWND hListview = GetDlgItem(hdlg, IDC_LIST1);
		ListView_SetExtendedListViewStyle(hListview, LVS_EX_GRIDLINES);

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

void Dlg_OnNotify(HWND hdlg, int idctrl, NMHDR *pnmhdr)
{
	if(LVN_GETDISPINFO==pnmhdr->code)
	{
		NMLVDISPINFO* plvdi = (NMLVDISPINFO*)pnmhdr;
		int iItem = plvdi->item.iItem;
		int iSubItem = plvdi->item.iSubItem;

		vaDbgTs(_T("LVN_GETDISPINFO: iItem=%d, iSubItem=%d"), iItem, iSubItem);

		switch (iSubItem)
		{
		case 0:
			plvdi->item.pszText = rgPetInfo[iItem].szKind;
			break;

		case 1:
			plvdi->item.pszText = rgPetInfo[iItem].szBreed;
			break;

		case 2:
			plvdi->item.pszText = rgPetInfo[iItem].szPrice;
			break;

		default:
			break;
		}
	}
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

	TCHAR textbuf[200];
	_sntprintf_s(textbuf, _TRUNCATE, _T("version: %d.%d.%d"), 
		ListView1_VMAJOR, ListView1_VMINOR, ListView1_VPATCH);
	SetDlgItemText(hdlg, IDC_LABEL1, textbuf);
	
	SetDlgItemText(hdlg, IDC_LIST1, prdata->mystr);

	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,0, IDC_LABEL1);
	jul->AnchorControl(0,0, 100,100, IDC_LIST1);
	jul->AnchorControl(0,100, 0,100, IDC_BUTTON1);

	HWND hListview = GetDlgItem(hdlg, IDC_LIST1);
	InitListviewColumns(hListview);
	InsertListviewItems(hListview);

	SetFocus(GetDlgItem(hdlg, IDC_BUTTON1));
	
	return 0; // Let Dlg-manager respect SetFocus().
}

INT_PTR WINAPI Dlg_Proc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		HANDLE_MSG(hdlg, WM_INITDIALOG,    Dlg_OnInitDialog);
		HANDLE_MSG(hdlg, WM_COMMAND,       Dlg_OnCommand);
		HANDLE_MSG(hdlg, WM_NOTIFY,        Dlg_OnNotify);
	}
	return FALSE;
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	const TCHAR *szfullcmdline = GetCommandLine();
	vaDbgTs(_T("GetCommandLine() = %s"), szfullcmdline);

	DlgPrivate_st dlgdata = { _T("Hello~. \r\nPrivate string here.") };

	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)&dlgdata);

	return 0;
}
