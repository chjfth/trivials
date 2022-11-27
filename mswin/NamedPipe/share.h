#ifndef __vaDbg_h_
#define __vaDbg_h_

#include <tchar.h>

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
};

void check_NamedPipeInfo(HANDLE hPipe, WhichSide_et side);


#endif
