#ifndef __vaDbg_h_
#define __vaDbg_h_

#include <tchar.h>

static const TCHAR *app_version = _T("1.1");

extern int g_read_timeout_msec;
extern int g_write_timeout_msec;

void PrnTs(const TCHAR *fmt, ...);

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false);

bool do_pipe_action(HANDLE hPipe, int key);

void do_interactive(HANDLE hPipe);

enum WhichSide_et
{
	ServerSide = 0,
	ClientSide = 1,
	Unset = 444,
};

extern WhichSide_et g_whichside;

void check_NamedPipeInfo(HANDLE hPipe);

//const TCHAR *Winerr2Str(DWORD winerr);

const TCHAR *WinerrStr(DWORD winerr=-1);

#endif
