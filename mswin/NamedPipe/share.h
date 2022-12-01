#ifndef __vaDbg_h_
#define __vaDbg_h_

#include <tchar.h>

static const TCHAR *app_version = _T("1.2");

extern int g_ReadFile_timeout;
extern int g_WriteFile_timeout;

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
