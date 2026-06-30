#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include "iversion.h"

#define CHHI_ALL_IMPL
#include <CHHI_DEBUG.h>
#include <vaDbgTs.h>
#include <CHHI_vaDBG_is_vaDbgTs.h>

// These .h are from __chjcxx git-module
#include <mswin/utils_env.h>
#include <mswin/utils_wingui.h>
#include <mswin/JULayout2.h>
#include <mswin/mmsystem.itc.h>
#include <fsapi.h>
using namespace fsapi;
#include <EnsureClnup_misc.h>
#include <IPlaySound_mswin.h>

#include "CxxDialogBase.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


HINSTANCE g_hinstExe;

class MainDialog : public CxxDialogBase
{
public:
	MainDialog(LPCTSTR mystr);
	~MainDialog();

	virtual INT_PTR DialogProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam);

private:
	void EnablePsObjButtons(HWND hdlg, bool is_enable);

private:
	IPlaySound *m_psobj;
	sdring_bytes m_wavbin;
};

MainDialog::MainDialog(LPCTSTR mystr)
{
	m_psobj = nullptr;
}

MainDialog::~MainDialog()
{
	delete m_psobj;
}

void MainDialog::EnablePsObjButtons(HWND hdlg, bool is_enable)
{
	const int arBtnUic[] = 
		{IDB_OpenWaveBin, IDB_OpenSoundFile, IDB_Play, IDB_Stop, IDB_Close};
	for(int i=0; i<ARRAYSIZE(arBtnUic); i++)
	{
		HWND hbtn = GetDlgItem(hdlg, arBtnUic[i]);
		Button_Enable(hbtn, is_enable);
	}
}

void MainDialog::OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify) 
{
	IPlaySound::ReCode_et pserr = IPlaySound::E_Unknown;
	HWND helog = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

	switch (id) 
	{{
	case IDB_CreateObj:
	{
		if(!m_psobj)
		{
			m_psobj = IPlaySound::CreateObj();
			vaSetDlgItemText(hdlg, IDE_ObjAddr, _T("0x%p"), m_psobj);

			EnablePsObjButtons(hdlg, true);
		}
		else
		{
			vaMsgBox(hdlg, 0, NULL, _T("This demo program cannot create two IPlaySound objects."));
		}
		break;
	}
	case IDB_DeleteObj:
	{
		delete m_psobj;
		m_psobj = nullptr;
		vaSetDlgItemText(hdlg, IDE_ObjAddr, _T(""));
		EnablePsObjButtons(hdlg, false);
		break;
	}
	case IDB_OpenWaveBin:
	{
		assert(m_psobj);
		
		Sdring filepath = sdrGetDlgItemText(hdlg, IDE_WaveBinFile);
		filehandle_t fh = file_open(filepath, open_for_read, open_share_both);
		if(fh<0)
		{
			vaMsgBox(hdlg, MB_ICONERROR, NULL_TITLE, 
				_T("Open file '%s' fail with %s."), filepath.c_str(), ITCSvn(fh, itc::fsapi_E_xxx));
			break;
		}
		CEC_filehandle_t cec_fh = fh;
		
		int filelen = (int)file_getsize(fh);
		assert(filelen>=0);

		m_wavbin.setbufsize(filelen);
		int nred = file_read(fh, m_wavbin.getbuf(), filelen);
		if(nred!=filelen)
		{
			vaMsgBox(hdlg, MB_ICONERROR, NULL_TITLE, 
				_T("Read file '%s' fail with %s."), filepath.c_str(), ITCSvn(nred, itc::fsapi_E_xxx));
			break;
		}

		pserr = m_psobj->OpenWavBin(m_wavbin.getptr(), filelen);
		if(pserr)
		{
			vaMsgBox(hdlg, MB_ICONERROR, NULL_TITLE, 
				_T("OpenWavBin() fail with %d."), pserr);
			break;
		}

		vaAppendLog_mled(helog, _T("OpenWavBin() success on '%s'"), filepath.c_str());

		break;
	}
	case IDB_OpenSoundFile:
	{
		assert(m_psobj);
		
		break;
	}
	case IDB_Close:
	{
		assert(m_psobj);
		
		m_psobj->Close();

		break;
	}
	case IDB_Play:
	{
		assert(m_psobj);

		pserr = m_psobj->PlayOnce();
		if(!pserr)
			vaAppendLog_mled(helog, _T("PlayOnce() returns ok"));
		else
			vaAppendLog_mled(helog, _T("PlayOnce() returns error: %d"), pserr);

		break;
	}
	case IDB_Stop:
	{
		assert(m_psobj);
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

	jul->AnchorControls(0,0, 0,0, 
		IDB_CreateObj, IDB_DeleteObj, IDE_ObjAddr,
		-1);
	jul->AnchorControls(0,0, 100,0, 
		IDE_WaveBinFile, IDE_SoundFile,
		-1);
	jul->AnchorControls(100,0, 100,0, 
		IDB_OpenWaveBin, IDB_OpenSoundFile,
		-1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
	
	jul->AnchorControls(0,100, 0,100, IDB_Play, IDB_Stop, -1);

	jul->AnchorControls(100,100, 100,100, IDB_Close, -1);
}

BOOL MainDialog::OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	vaSetWindowText(hdlg, _T("tPlaySound v%d.%d.%d"), 
		tPlaySound_VMAJOR, tPlaySound_VMINOR, tPlaySound_VPATCH);
	
	SetDlgItemText(hdlg, IDE_WaveBinFile, _T("foxdog-8bit.wav"));
	SetDlgItemText(hdlg, IDE_SoundFile, _T("chime-2sec.mp3"));

	Dlg_EnableJULayout(hdlg);

	EnablePsObjButtons(hdlg, false);

	SetFocus(GetDlgItem(hdlg, IDB_CreateObj));
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
