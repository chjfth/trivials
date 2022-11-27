#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <conio.h>

#include "share.h"

int g_read_timeout_msec = 5000;
int g_write_timeout_msec = 5000;


template<typename T1, typename T2>
bool IsSameBool(T1 a, T2 b)
{
	if(a && b)
		return true;
	else if(!a && !b)
		return true;
	else
		return false;
}


void PrnTs(const TCHAR *fmt, ...)
{
	static int count = 0;
	static DWORD s_prev_msec = GetTickCount();

	DWORD now_msec = GetTickCount();

	TCHAR buf[1000] = {0};

	// Print timestamp to show that time has elapsed for more than one second.
	DWORD delta_msec = now_msec - s_prev_msec;
	if(delta_msec>=1000)
	{
		_tprintf(_T(".\n"));
	}

	TCHAR timebuf[40] = {};
	now_timestr(timebuf, ARRAYSIZE(timebuf));

	_sntprintf_s(buf, _TRUNCATE, _T("%s (+%3u.%03us) "), timebuf, 
		delta_msec/1000, delta_msec%1000);

	int prefixlen = (int)_tcslen(buf);

	va_list args;
	va_start(args, fmt);

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);

	va_end(args);

	_tprintf(_T("%s\n"), buf);

	s_prev_msec = now_msec;
}

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd)
{
	SYSTEMTIME st = {0};
	GetLocalTime(&st);
	buf[0]=_T('['); buf[1]=_T('\0'); buf[bufchars-1] = _T('\0');
	if(ymd) {
		_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%04d-%02d-%02d "), buf, 
			st.wYear, st.wMonth, st.wDay);
	}
	_sntprintf_s(buf, bufchars-1, _TRUNCATE, _T("%s%02d:%02d:%02d.%03d]"), buf,
		st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);
	return buf;
}

void do_pipeWrite(HANDLE hPipe, int bytes_towr)
{
	char *wrbuf = new char[bytes_towr+1];
	_snprintf_s(wrbuf, bytes_towr+1, _TRUNCATE, "0123456789");

	OVERLAPPED ovlp = {};
	ovlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	PrnTs(_T("WriteFile() to pipe, %d bytes..."), bytes_towr);

	DWORD nWrDone = 0;
	BOOL succ = WriteFile(hPipe, wrbuf, bytes_towr, &nWrDone, &ovlp);
	DWORD winerr = GetLastError();
	if(!succ && winerr==ERROR_IO_PENDING)
	{
		PrnTs(_T("  Async wait (%d millsec)..."), g_write_timeout_msec);
		
		DWORD waitre = WaitForSingleObject(ovlp.hEvent, g_write_timeout_msec);
		if(waitre==WAIT_TIMEOUT)
		{
			PrnTs(_T("  Wait timeout. Now calling CancelIoEx()..."));
			succ = CancelIoEx(hPipe, &ovlp);
			winerr = GetLastError();

			PrnTs(_T("  CancelIoEx() done."));

			assert(succ || winerr==ERROR_NOT_FOUND);
			// -- ERROR_NOT_FOUND actually means we wrote some bytes just before 
			// we are going to CancelIoEx(), it's actually a success.
		}
		else
			assert(waitre==WAIT_OBJECT_0);
		
		succ = GetOverlappedResult(hPipe, &ovlp, &nWrDone, TRUE);
	}

	if(succ)
	{
		PrnTs(_T("WriteFile() success, %d bytes."), nWrDone);
	}
	else
	{
		winerr = GetLastError();
		PrnTs(_T("WriteFile() fail, winerr=%d"), winerr);
	}

	CloseHandle(ovlp.hEvent);
	delete wrbuf;
}

void do_pipeRead(HANDLE hPipe, int bytes_tord)
{
	char *rdbuf = new char[bytes_tord+1];
	rdbuf[0] = 0;

	OVERLAPPED ovlp = {};
	ovlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	PrnTs(_T("ReadFile() from pipe, %d bytes..."), bytes_tord);

	DWORD nRdDone = 0;
	BOOL succ = ReadFile(hPipe, rdbuf, bytes_tord, &nRdDone, &ovlp);
	DWORD winerr = GetLastError();
	if(!succ && winerr==ERROR_IO_PENDING)
	{
		PrnTs(_T("  Async wait (%d millisec)..."), g_read_timeout_msec);
		
		DWORD waitre = WaitForSingleObject(ovlp.hEvent, g_read_timeout_msec);
		if(waitre==WAIT_TIMEOUT)
		{
			PrnTs(_T("  Wait timeout. Now calling CancelIoEx()..."));

			succ = CancelIoEx(hPipe, &ovlp);
			winerr = GetLastError();

			PrnTs(_T("  CancelIoEx() done."));

			assert(succ || winerr==ERROR_NOT_FOUND); 
			// -- ERROR_NOT_FOUND actually means we got bytes just before 
			// we are going to CancelIoEx(), it's actually a success.
		}
		else 
			assert(waitre==WAIT_OBJECT_0);
		
		succ = GetOverlappedResult(hPipe, &ovlp, &nRdDone, TRUE);
	}

	if(succ)
	{
		PrnTs(_T("ReadFile() success, %d bytes."), nRdDone);
	}
	else
	{
		winerr = GetLastError();
		PrnTs(_T("ReadFile() fail, winerr=%d"), winerr);
	}

	CloseHandle(ovlp.hEvent);
	delete rdbuf;
}

bool do_pipe_action(HANDLE hPipe, int key)
{
	static int s_bytes_towr = 10;
	static int s_bytes_tord = 10;

	BOOL succ = 0;
	DWORD winerr = 0;
	char ibuf[20] = {};


	if(key=='w')
	{
		do_pipeWrite(hPipe, s_bytes_towr);
	}
	else if(key=='r')
	{
		do_pipeRead(hPipe, s_bytes_tord);
	}
	else if(key=='W')
	{
		_tprintf(_T("How many bytes to write once? "));

		ibuf[0] = '\0';
		fgets(ibuf, sizeof(ibuf), stdin);
		int num = atoi(ibuf);
		if(num>0)
		{
			_tprintf(_T("Bytes-to-write once set to %d.\n"), num);
			s_bytes_towr = num;
		}
	}
	else if(key=='R')
	{

	}
	else
	{
		_tprintf(_T("Invalid action.\n"));
		return false;
	}

	return true;
}

void do_interactive(HANDLE hPipe)
{
	_tprintf(_T("Select: (w)write, (r)read, W(bytes-to-write), R(bytes-to-read), 0(quit)\n"));
	int key = _getch();

	for(; ;)
	{
		if(key=='0')
			break;

		do_pipe_action(hPipe, key);

		_tprintf(_T("Select: "));
		key = _getch();
		if(key!='\r')
			_tprintf(_T("%c\n"), key);
	}
}


void check_NamedPipeInfo(HANDLE hPipe, WhichSide_et side)
{
	DWORD winerr = 0;
	BOOL succ = 0;

	DWORD piflags = 0;
	succ = GetNamedPipeInfo(hPipe, &piflags, NULL, NULL, NULL);
	assert(succ && IsSameBool(piflags&PIPE_SERVER_END, side==ServerSide));

	DWORD pipestate = 0;
	DWORD nCurInstances = 0;
	DWORD nWrbufThreshold = 0;
	DWORD nWrbufStaymsec = 0;
	TCHAR szPeerUsername[256] = {};

	succ = GetNamedPipeHandleState(hPipe, &pipestate, &nCurInstances,
		0,0, 0,0);

	if(!succ)
	{
		_tprintf(_T("GetNamedPipeHandleState() fail, winerr=%d\n"), GetLastError());
		return;
	}

	const TCHAR *szpipestate = _T("PIPE_READMODE_BYTE | PIPE_WAIT");
	if(pipestate==(PIPE_NOWAIT|PIPE_READMODE_MESSAGE))
		szpipestate = _T("PIPE_READMODE_MESSAGE | PIPE_NOWAIT");
	else if(pipestate==PIPE_NOWAIT)
		szpipestate = _T("PIPE_READMODE_BYTE | PIPE_NOWAIT");
	else if(pipestate==PIPE_READMODE_MESSAGE)
		szpipestate = _T("PIPE_READMODE_MESSAGE | PIPE_WAIT");

	_tprintf(_T("GetNamedPipeHandleState() returns:\n"));
	_tprintf(_T("  pipestate       = 0x%X (%s)\n"), pipestate, szpipestate);
	_tprintf(_T("  nCurInstances   = %d\n"), nCurInstances);

	if(side==ServerSide)
	{
		succ = GetNamedPipeHandleState(hPipe, NULL, NULL,
			NULL, NULL,
			szPeerUsername, ARRAYSIZE(szPeerUsername)-1);
		if(succ)
		{
			_tprintf(_T("  Peer username   : %s\n"), szPeerUsername);
		}
		else
		{
			winerr = GetLastError();
			if(winerr==ERROR_CANNOT_IMPERSONATE)
			{
				_tprintf(_T("  Peer username   : N/A, client-side no SECURITY_IMPERSONATION\n"));
			}
		}
	}
	else
	{
		assert(side==ClientSide);

		succ = GetNamedPipeHandleState(hPipe, NULL, NULL,
			&nWrbufThreshold, &nWrbufStaymsec, 
			NULL, NULL);
		if(succ)
		{
			_tprintf(_T("  nWrbufThreshold = %d\n"), nWrbufThreshold);
			_tprintf(_T("  nWrbufMillisec  = %d\n"), nWrbufStaymsec);
		}
		else
		{
			winerr = GetLastError();
			if(winerr==ERROR_INVALID_PARAMETER)
			{
				_tprintf(_T("  nWrbufThreshold = N/A (Pipe server-side is on local machine)\n"));
				_tprintf(_T("  nWrbufMillisec  = N/A (Pipe server-side is on local machine)\n"));
			}
		}
	}

	// not using GetNamedPipeInfo
}

