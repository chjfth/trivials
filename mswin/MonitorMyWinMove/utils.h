#pragma once

#include <tchar.h>
#include <assert.h>

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false);

void vaDbgTs(const TCHAR *fmt, ...);

void vaDbgS(const TCHAR *fmt, ...);

void AppendText_mled(HWND hedit, const TCHAR *text);

//void vaAppendText_mled(HWND hedit, const TCHAR *szfmt, ...);
void vaSetDlgItemText(HWND hwnd, UINT id_edit, const TCHAR *szfmt, ...);

