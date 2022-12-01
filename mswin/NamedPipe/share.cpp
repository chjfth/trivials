#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <conio.h>

#include "share.h"

WhichSide_et g_whichside = Unset;

int g_ReadFile_timeout = 5000;
int g_WriteFile_timeout = 5000;


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

	TCHAR c_whichside = ' ';
	if(g_whichside==ServerSide)
		c_whichside = 'S';
	else if(g_whichside==ClientSide)
		c_whichside = 'C';

	TCHAR timebuf[40] = {};
	now_timestr(timebuf, ARRAYSIZE(timebuf));

	_sntprintf_s(buf, _TRUNCATE, _T("%c%s (+%3u.%03us) "), 
		c_whichside,
		timebuf, 
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
		PrnTs(_T("  Async wait (%d millsec)..."), g_WriteFile_timeout);
		
		DWORD waitre = WaitForSingleObject(ovlp.hEvent, g_WriteFile_timeout);
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
		PrnTs(_T("WriteFile() fail, %s"), WinerrStr());
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
		PrnTs(_T("  Async wait (%d millisec)..."), g_ReadFile_timeout);
		
		DWORD waitre = WaitForSingleObject(ovlp.hEvent, g_ReadFile_timeout);
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
		PrnTs(_T("ReadFile() fail, %s"), WinerrStr());
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
	char keysbuf[20] = {};


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
		_tprintf(_T("How many bytes to write once? (%d) "), s_bytes_towr);

		keysbuf[0] = '\0';
		fgets(keysbuf, sizeof(keysbuf)-1, stdin);
		int num = atoi(keysbuf);
		if(num>0)
		{
			_tprintf(_T("Bytes-to-write once set to %d.\n"), num);
			s_bytes_towr = num;
		}
	}
	else if(key=='R')
	{
		_tprintf(_T("How many bytes to request on each read? (%d)"), s_bytes_tord);

		keysbuf[0] = '\0';
		fgets(keysbuf, sizeof(keysbuf)-1, stdin);
		int num = atoi(keysbuf);
		if(num>0)
		{
			_tprintf(_T("Bytes-to-read once set to %d.\n"), num);
			s_bytes_tord = num;
		}
	}
	else if(key=='f')
	{
		PrnTs(_T("Calling FlushFileBuffers() , blocking API..."));
		succ = FlushFileBuffers(hPipe);
		if(succ)
			PrnTs(_T("Done    FlushFileBuffers() ."));
		else
			PrnTs(_T("FlushFileBuffers() fail, %s"), WinerrStr());
	}
	else if(key=='\r')
	{
		_tprintf(_T("\n"));
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
//	BOOL succ = 0;
	int key = 0;
	_tprintf(_T("Select: (w)write (r)read W(bytes-to-write) R(bytes-to-read) f(Flush) 0(quit) "));
	goto GETCH;

	for(; ;)
	{
		if(key=='0')
		{
			break;
		}

		do_pipe_action(hPipe, key);

		_tprintf(_T("Select: "));
GETCH:
		key = _getch();
		if(key!='\r')
			_tprintf(_T("%c\n"), key);
	}
}


void check_NamedPipeInfo(HANDLE hPipe)
{
	DWORD winerr = 0;
	BOOL succ = 0;

	DWORD piflags = 0;
	DWORD pi_obufsize = 0, pi_ibufsize = 0, pi_maxinstance;
	succ = GetNamedPipeInfo(hPipe, &piflags, 
		&pi_obufsize, &pi_ibufsize, &pi_maxinstance);
	assert(succ && IsSameBool(piflags&PIPE_SERVER_END, g_whichside==ServerSide));

	const TCHAR *server_pers = (piflags&PIPE_SERVER_END) ? _T("") : _T("(server-side perspective)");

	_tprintf(_T("GetNamedPipeInfo() returns:\n"));
	_tprintf(_T("  Pipe type     : %s\n"), piflags&PIPE_TYPE_MESSAGE ? _T("Message pipe") : _T("Stream pipe"));
	_tprintf(_T("  OutBufferSize : %-8d %s\n"), pi_obufsize, server_pers);
	_tprintf(_T("   InBufferSize : %-8d %s\n"), pi_ibufsize, server_pers);
	_tprintf(_T("  Max instances : %d\n"), pi_maxinstance);

	DWORD pipestate = 0;
	DWORD nCurInstances = 0;
	DWORD nWrbufThreshold = 0;
	DWORD nWrbufStaymsec = 0;
	TCHAR szPeerUsername[256] = {};

	succ = GetNamedPipeHandleState(hPipe, &pipestate, &nCurInstances,
		0,0, 0,0);

	if(!succ)
	{
		_tprintf(_T("GetNamedPipeHandleState() fail, %s\n"), WinerrStr());
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

	if(g_whichside==ServerSide)
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
		assert(g_whichside==ClientSide);

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



//////////////////////////////////////////////////////////////////////////

struct Const2Str_st
{
	int Const;
	const TCHAR *Str;
};

#define ITEM_Const2Str(macroname) { macroname, _T( #macroname ) }

template<size_t arsize>
const TCHAR *Const2Str(
	const Const2Str_st (&armap)[arsize], int Const, bool ret_known=false)
{
	for(int i=0; i<arsize; i++)
	{
		if(armap[i].Const==Const)
			return armap[i].Str;
	}

	return ret_known ? _T("Unknown") : NULL;
}

////

#include "winerrs.partial.cpp"

const TCHAR * Winerr2Str(DWORD winerr)
{
	return Const2Str(gar_Winerr2Str, winerr, true);
}

const TCHAR *WinerrStr(DWORD winerr)
{
	static TCHAR s_retbuf[80] = {};
	s_retbuf[0] = 0;

	if (winerr == (DWORD)-1)
		winerr = GetLastError();

	const TCHAR *errstr = Const2Str(gar_Winerr2Str, winerr, false);

	if(errstr)
	{
		_sntprintf_s(s_retbuf, _TRUNCATE, _T("WinErr=%d (%s)"), winerr, errstr);
	}
	else
	{
		TCHAR szErrDesc[200] = {};
		DWORD retchars = FormatMessage(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, 
			winerr,
			MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), // LANGID
			szErrDesc, ARRAYSIZE(szErrDesc)-1,
			NULL); 

		if(retchars>0)
		{
			_sntprintf_s(s_retbuf, _TRUNCATE, _T("WinErr=%d, %s"), winerr, szErrDesc);
		}
		else
		{
			_sntprintf_s(s_retbuf, _TRUNCATE, 
				_T("WinErr=%d (FormatMessage does not known this error-code)"), 
				winerr);
		}
	}

	return s_retbuf;
}
