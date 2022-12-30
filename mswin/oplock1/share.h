#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <assert.h>
#include <windows.h>

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false);

void PrnTs(const TCHAR *fmt, ...);


