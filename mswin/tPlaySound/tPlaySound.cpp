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

#include <mswin/WinError.itc.h>
#include <mswin/WinUser.itc.h>
#include <mswin/mmsystem.itc.h>
#include "AutoNestCount.h"
#include "CxxDialogBase.h"

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

namespace itc {
ITC_MAKE_OBJECT(itc_WM_app_all, 
	new Enum2Val_merge(
		_e2v_MM_xxx_winmsg, N_e2v_MM_xxx_winmsg,
		_e2v_WM_xxx, N_e2v_WM_xxx,
		nullptr, 0) // this Enum2Val_merge object will not be deleted, by design
	, ITCF_HEX1B)
} // itc

bool g_isEdm = false; // EDM special

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
		UINT msgval = CPlaySound_u::RegisterHwndNotify(m_psobj, hdlg);
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
		CPlaySound_u::RegisterHwndNotify(m_psobj, NULL);
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

		pserr = m_psobj->OpenSoundBin(m_wavbin.getptr(), filelen);
		if(pserr)
		{
			vaMsgBox(hdlg, MB_ICONERROR, NULL_TITLE, 
				_T("OpenWavBin() fail with %s"), ITCSvn(pserr, IPlaySound::ReCode_itc));
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
			vaAppendLog_mled(helog, _T("OpenSoundFile() fail with %s"), ITCSvn(pserr, IPlaySound::ReCode_itc));

		break;
	}
	case IDB_CloseDev:
	{
		assert(m_psobj);
		
		pserr = m_psobj->Close();
		vaAppendLog_mled(helog, _T("Close() done, ret=%s"), ITCSvn(pserr, IPlaySound::ReCode_itc));

		break;
	}
	case IDB_Play:
	{
		assert(m_psobj);

		pserr = m_psobj->PlayOnce();
		if(!pserr)
			vaAppendLog_mled(helog, _T("PlayOnce() ok"));
		else
			vaAppendLog_mled(helog, _T("PlayOnce() fail with %s"), ITCSvn(pserr, IPlaySound::ReCode_itc));

		break;
	}
	case IDB_Stop:
	{
		assert(m_psobj);
		pserr = m_psobj->Stop();

		if(!pserr)
			vaAppendLog_mled(helog, _T("Stop() ok"));
		else
			vaAppendLog_mled(helog, _T("Stop() fail with %s"), ITCSvn(pserr, IPlaySound::ReCode_itc));
		
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
		util_PostEndDialogMessage(hdlg);
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
	util_SetWindowIcon(hdlg, MAKEINTRESOURCE(IDI_WINMAIN));

	vaSetWindowText(hdlg, _T("tPlaySound v%d.%d.%d %s"), 
		tPlaySound_VMAJOR, tPlaySound_VMINOR, tPlaySound_VPATCH,
		g_isEdm ? _T("(EDM mode)" : _T(""))
		);
	
	SetDlgItemText(hdlg, IDE_WaveBinFile, _T("foxdog-8bit.wav"));
	SetDlgItemText(hdlg, IDE_SoundFile, _T("chime-2sec.mp3"));

	Button_SetCheck(GetDlgItem(hdlg, IDCK_NotifyPlaydone), BST_CHECKED);

	Dlg_EnableJULayout(hdlg);

	EnablePsObjButtons(false);

	if(!g_isEdm)
	{
		HWND helog = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);
		vaSetWindowText(helog, _T("%s")
			,
			_T("Hint: To run the dialog in EDM(EndDialog-message) mode, pass \"peek\" parameter.\r\n")
			_T("\r\n")
			_T("In EDM mode, this EXE code manages the message loop and all WM_xxx messages are dumped to debug channel.\r\n")
			);
	}

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

	if(uMsg==0)
	{
		// [2026-07-08] Chj note: When managing this dlgbox in EDM(EndDialog-message) mode,
		// and if we close the dlgbox, we can once see uMsg==0 here.
		// Verified on WinXP & Win10.
		vaDbgTs(_T("[Note] In DialogProc(), we see uMsg==0. Just ignore it."));
		return TRUE;
	}

	if(uMsg==m_msgNotifyPlayDone)
	{
		HWND helog = GetDlgItem(hdlg, IDC_EDIT_LOGMSG);

		auto playdone = (CPlaySound_u::PlayDone_et)wParam;
		assert(playdone==CPlaySound_u::PlayDone_Success || playdone==CPlaySound_u::PlayDone_Aborted);
		
		IPlaySound::PlayingQuery_et isplaying = m_psobj->IsPlaying();
		assert(isplaying!=IPlaySound::PlayingQuery_NotSupport);
		// -- If PlayingQuery_NotSupport, this notification message could NOT happen.

		if(isplaying==IPlaySound::Playing)
		{
			vaAppendLog_mled(helog, _T("Play %s!! (delayed previous)"),
				playdone==CPlaySound_u::PlayDone_Success ? _T("done") : _T("aborted")
				);
		}
		else if(isplaying==IPlaySound::Stopped)
		{
			vaAppendLog_mled(helog, _T("Play %s!! (true stop)"),
				playdone==CPlaySound_u::PlayDone_Success ? _T("done") : _T("aborted")
				);
		}
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


int g_nestlv = 0; // WM_xxx nested level
int g_msgcount = 0;
bool g_inside_getmessage = false;

bool WinMain_PeekDlgBox(HINSTANCE hinstExe)
{
	MainDialog dlg(_T("Hello.\r\nPrivate string here."));
	g_isEdm = true;
	HWND hdlg = dlg.CreateModeless(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL);
	assert(hdlg);
	if(!IsWindow(hdlg))
		return false;

	ShowWindow(hdlg, SW_SHOWNORMAL);

	TCHAR szClassName[40] = {};
	MSG msg = {};
	while(1) 
	{
		vaDbgTs(_T("GetMessage() >>>"));

		g_inside_getmessage = true;
		BOOL succMsg = GetMessage(&msg, NULL, 0, 0);
		g_inside_getmessage = false;
		
		// Check msg.hwnd's traits
		HWND hwnd_mytop = hdlg;
		HWND hwnd_root = GetAncestor(msg.hwnd, GA_ROOT);
		const TCHAR *trait = _T("");
		if(msg.hwnd==hwnd_mytop)
			trait = _T("top");
		else if(hwnd_root==hwnd_mytop)
			trait = _T("child");
		else
		{
			// Other top-level window, it could be a class="MSCTFIME UI"
			trait = _T("other");
		}

		szClassName[0] = '\0';
		GetClassName(msg.hwnd, szClassName, ARRAYSIZE(szClassName));

		vaDbgTs(_T("GetMessage() <<< got %s (hwnd=0x%X \"%s\") %s"), 
			ITCSnv(msg.message, itc::itc_WM_app_all), 
			Ptr2Uint(msg.hwnd), szClassName, // (hwnd=0x%X "%s")
			trait // %s
			);

		if(util_IsEndDialog(hdlg, msg))
		{
			// We get dlgbox kill request, so kill it.
			BOOL succ = DestroyWindow(hdlg);
			if(succ)
				vaDbgTs(_T("DestroyWindow(0x%X) success."), Ptr2Uint(hdlg));
			else
				vaDbgTs(_T("DestroyWindow(0x%X) fail! WinErr=%s"), Ptr2Uint(hdlg), ITCS_WinError);
			break; 
		}

		if(!succMsg)
			break;

		vaDbgTs(_T("IsDialogMessage() >>>"));

		BOOL isdlgmsg = IsDialogMessage(hdlg, &msg);

		vaDbgTs(_T("IsDialogMessage() <<< %s"), isdlgmsg?_T("(was yes)"):_T(""));

		if(isdlgmsg)
			continue;

		TranslateMessage (&msg) ;
		DispatchMessage  (&msg) ;
	}

	assert(!IsWindow(hdlg));

	return true;
}

int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR szParams, int) 
{
	g_hinstExe = hinstExe;

	InitCommonControls(); // WinXP requires this, to work with Visual-style manifest

	int argc = __argc;
#ifdef UNICODE
	PCTSTR* argv = (PCTSTR*) CommandLineToArgvW(GetCommandLine(), &argc);
#else
	PCTSTR* argv = (PCTSTR*) __argv;
#endif
	// Use argc and argv[] below, just like what traditional main() does.

	if(argc>1 && _tcscmp(argv[1], _T("peek"))==0)
	{
		// Chj Special: Use modeless dialog box.
		// With exe parameter "peek", we manage this dlgbox in EDM(EndDialog-message) mode.
		// We will use our own message-pump code to peek WM_xxx flowing through.
		WinMain_PeekDlgBox(hinstExe);
	}
	else
	{
		// Use traditional Modal dialog box.
		MainDialog dlg(_T("Hello.\r\nPrivate string here."));
		dlg.DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL);
	}

	return 0;
}
