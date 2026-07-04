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
#include <InterpretConst.h>
  using namespace itc;
#include <mswin/win32cozy.h>
#include <mswin/utils_env.h>
#include <mswin/utils_wingui.h>
#include <mswin/JULayout2.h>
#include <mswin/mmsystem.itc.h>
#include <fsapi.h>
  using namespace fsapi;
#include <EnsureClnup_misc.h>
#include <mswin/IPlaySound_mswin.h>

#include "CxxDialogBase.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")


HINSTANCE g_hinstExe;

class MainDialog : public CxxDialogBase
{
public:
	MainDialog(LPCTSTR mystr);
	~MainDialog();

	virtual INT_PTR DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	void OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);
	BOOL OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam);

private:
	void EnablePsObjButtons(bool is_enable);

	void Register_PlaydoneNotify(bool want_notify);

private:
	IPlaySound *m_psobj;
	sdring_bytes m_wavbin;

	UINT m_msgNotifyPlayDone;
};

MainDialog::MainDialog(LPCTSTR mystr)
{
	m_psobj = nullptr;
	m_msgNotifyPlayDone = FALSE;
}

MainDialog::~MainDialog()
{
	delete m_psobj;
}

void MainDialog::EnablePsObjButtons(bool is_enable)
{
	HWND hdlg = m_hwndDlg;

	const int arBtnUic[] = {
		IDB_DeleteObj, IDCK_NotifyPlaydone, IDB_OpenWaveBin, IDB_OpenSoundFile, 
		IDB_Play, IDB_Stop, IDB_CloseDev, IDB_StopReplay, IDB_PlayStopQuick
	};

	Enable_Uics(is_enable, hdlg, arBtnUic);
}

void MainDialog::Register_PlaydoneNotify(bool want_notify)
{
	assert(m_psobj);

	HWND hdlg = m_hwndDlg;
	HWND helog = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
	HWND hbtn = GetDlgItem(hdlg, IDCK_NotifyPlaydone);

	if(want_notify)
	{
		UINT msgval = IPlaySound_RegisterHwndNotify(m_psobj, hdlg);
		if(msgval!=FALSE)
		{
			vaAppendLog_mled(helog, _T("Playdone-notify enabled, msgvalue=%d(=0x%X)"), msgval, msgval);
			m_msgNotifyPlayDone = msgval;
			Button_SetCheck(hbtn, BST_CHECKED);
		}
		else
		{
			vaAppendLog_mled(helog, _T("Panic! IPlaySound_RegisterHwndNotify() fails!"));
			Button_SetCheck(hbtn, BST_UNCHECKED);
		}
	}
	else
	{
		IPlaySound_RegisterHwndNotify(m_psobj, NULL);
		vaAppendLog_mled(helog, _T("Playdone-notify disabled"));
		Button_SetCheck(hbtn, BST_UNCHECKED);
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
			vaSetDlgItemText(hdlg, IDE_ObjAddr, _T("objaddr=0x%p"), m_psobj);

			Register_PlaydoneNotify(true);

			EnablePsObjButtons(true);
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
		EnablePsObjButtons(false);
		m_msgNotifyPlayDone = FALSE;
		break;
	}
	case IDCK_NotifyPlaydone:
	{
		if(codeNotify==BN_CLICKED)
		{
			HWND hbtn = GetDlgItem(hdlg, IDCK_NotifyPlaydone);
			if(Button_GetCheck(hbtn))
				Register_PlaydoneNotify(false);
			else
				Register_PlaydoneNotify(true);
		}
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
				_T("OpenWavBin() fail with %s"), ITCSvn(pserr, IPlaySound_ReCode));
			break;
		}

		vaAppendLog_mled(helog, _T("OpenWavBin() success on '%s'"), filepath.c_str());

		break;
	}
	case IDB_OpenSoundFile:
	{
		assert(m_psobj);
		
		Sdring filepath = sdrGetDlgItemText(hdlg, IDE_SoundFile);
		pserr = m_psobj->OpenSoundFile(filepath);
		if(!pserr)
			vaAppendLog_mled(helog, _T("OpenSoundFile() success on '%s'"), filepath.c_str());
		else
			vaAppendLog_mled(helog, _T("OpenSoundFile() fail with %s"), ITCSvn(pserr, IPlaySound_ReCode));

		break;
	}
	case IDB_CloseDev:
	{
		assert(m_psobj);
		
		pserr = m_psobj->Close();
		vaAppendLog_mled(helog, _T("Close() done, ret=%s"), ITCSvn(pserr, IPlaySound_ReCode));

		break;
	}
	case IDB_Play:
	{
		assert(m_psobj);

		pserr = m_psobj->PlayOnce();
		if(!pserr)
			vaAppendLog_mled(helog, _T("PlayOnce() ok"));
		else
			vaAppendLog_mled(helog, _T("PlayOnce() fail with %s"), ITCSvn(pserr, IPlaySound_ReCode));

		break;
	}
	case IDB_Stop:
	{
		assert(m_psobj);
		pserr = m_psobj->Stop();

		if(!pserr)
			vaAppendLog_mled(helog, _T("Stop() ok"));
		else
			vaAppendLog_mled(helog, _T("Stop() fail with %s"), ITCSvn(pserr, IPlaySound_ReCode));
		
		break;
	}
	case IDB_StopReplay:
	{
		vaAppendLog_mled(helog, _T("[Stop + Replay] calling start."));
		pserr = m_psobj->Stop();
		pserr = m_psobj->PlayOnce();
		vaAppendLog_mled(helog, _T("[Stop + Replay] calling returned."));
		break;
	}
	case IDB_PlayStopQuick:
	{
		vaAppendLog_mled(helog, _T("[Play+Stop quick] calling start."));
		pserr = m_psobj->PlayOnce();
		pserr = m_psobj->Stop();
		vaAppendLog_mled(helog, _T("[Play+Stop quick] calling returned."));
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
		IDB_OpenWaveBin, IDB_OpenSoundFile, IDB_CloseDev, IDCK_NotifyPlaydone,
		-1);
	jul->AnchorControl(0,0, 100,100, IDC_EDIT_LOGMSG);
	
	jul->AnchorControls(0,100, 0,100, IDB_Play, IDB_Stop, 
		IDB_StopReplay, IDB_PlayStopQuick,
		-1);
}

BOOL MainDialog::OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	SNDMSG(hdlg, WM_SETICON, TRUE, (LPARAM)LoadIcon(GetWindowInstance(hdlg), MAKEINTRESOURCE(IDI_WINMAIN)));

	vaSetWindowText(hdlg, _T("tPlaySound v%d.%d.%d"), 
		tPlaySound_VMAJOR, tPlaySound_VMINOR, tPlaySound_VPATCH);
	
	SetDlgItemText(hdlg, IDE_WaveBinFile, _T("foxdog-8bit.wav"));
	SetDlgItemText(hdlg, IDE_SoundFile, _T("chime-2sec.mp3"));

	Button_SetCheck(GetDlgItem(hdlg, IDCK_NotifyPlaydone), BST_CHECKED);

	Dlg_EnableJULayout(hdlg);

	EnablePsObjButtons(false);

	SetFocus(GetDlgItem(hdlg, IDB_CreateObj));
	return FALSE; // FALSE to let Dlg-manager respect our SetFocus().
}

INT_PTR MainDialog::DialogProc(UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	HWND hdlg = m_hwndDlg;

	switch (uMsg) 
	{
		HANDLE_dlgMSG(hdlg, WM_INITDIALOG,    OnInitDialog);
		HANDLE_dlgMSG(hdlg, WM_COMMAND,       OnCommand);
	}

	if(uMsg==m_msgNotifyPlayDone)
	{
		HWND helog = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

		IPlaySound::PlayingQuery_et isplaying = m_psobj->IsPlaying();

		assert(isplaying!=IPlaySound::PlayingQuery_NotSupport);
		// -- If PlayingQuery_NotSupport, this notification message could NOT happen.

		if(isplaying==IPlaySound::Playing)
			vaAppendLog_mled(helog, _T("Play done!! (delayed previous)"));
		else if(isplaying==IPlaySound::Stopped)
			vaAppendLog_mled(helog, _T("Play done!! (true stop)"));
		else
			vaAppendLog_mled(helog, _T("Panic!!! Weird play-done!!!"));

		return TRUE;
	}

	/* No need, just for test
	if(uMsg==MM_MCINOTIFY)
	{
		vaDbgTs(_T("EXE got MM_MCINOTIFY, devid=%d , notifycode=%s"), 
			(LONG)lParam, ITCSvn((LONG)wParam, itc::MCI_NOTIFY_xxx));
	}
	*/

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
