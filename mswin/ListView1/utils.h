#pragma once

#include <tchar.h>

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false);

void vaDbgTs(const TCHAR *fmt, ...);

void vaDbgS(const TCHAR *fmt, ...);
