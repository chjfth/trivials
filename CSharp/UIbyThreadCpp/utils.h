#define WIN32_LEAN_AND_MEAN
#include <windows.h>

typedef int(*PROC_WinThread)(void *param);
// Note: This defaults to __cdecl, not __stdcall .

HANDLE winCreateThread(PROC_WinThread proc, void *param, int stacksize = 0, unsigned *pThreadId = 0);

DWORD winWaitThreadEnd(HANDLE hThread, int timeout_millisec);
