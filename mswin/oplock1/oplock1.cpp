#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <assert.h>
#include <conio.h>
#include <process.h>
#include <windows.h>

// Sample code according to: 
// https://devblogs.microsoft.com/oldnewthing/20130415-00/?p=4663
// Using opportunistic locks to get out of the way if somebody, April 2013.

// This program shows: When we are using a file(having a file handle opened), 
// we can tell system to notify our code when some other client later tries to
// open the same file.
// This notification is achieved by our putting an oplock on the file handle
// and "wait" for the oplock to be broken. When some other client tries to
// open the same file, Windows system proactively breaks the oplock and notify us.

TCHAR* now_timestr(TCHAR buf[], int bufchars, bool ymd=false)
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

	_sntprintf_s(buf, _TRUNCATE, _T("%s (+%3u.%03us) "), 
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

//////////////////////////////////////////////////////////////////////////

void _thread_waitkey(void *)
{
	while(_getch()!='\r');
}

void place_oplock_and_wait_broken(HANDLE hfile, DWORD oplock_level)
{
	OVERLAPPED ovlp = {};
	ovlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	REQUEST_OPLOCK_INPUT_BUFFER inp =
	{
		REQUEST_OPLOCK_CURRENT_VERSION, // 1
		sizeof(inp),
		oplock_level,
		REQUEST_OPLOCK_INPUT_FLAG_REQUEST
	};

	REQUEST_OPLOCK_OUTPUT_BUFFER outp = 
		{ REQUEST_OPLOCK_CURRENT_VERSION, sizeof(outp) };

	PrnTs(_T("Issuing FSCTL_REQUEST_OPLOCK on the file handle(0x%08X)."), hfile);

	DWORD retbytes = 0;
	BOOL succ = DeviceIoControl(hfile, FSCTL_REQUEST_OPLOCK,
		&inp, sizeof(inp),
		&outp, sizeof(outp), 
		&retbytes, &ovlp);
	DWORD winerr = GetLastError();
	assert(!succ);
	if(winerr!=ERROR_IO_PENDING)
	{
		PrnTs(_T("[UNEXPECT!] Got WinErr=%d, should be ERROR_IO_PENDING."), winerr);
		exit(4);
	}

	PrnTs(_T("OK. We have placed an oplock on the file handle, ")
		_T("and now waiting for the lock to be broken.\n")
		_T("Use another program to access the same file to see the oplock broken,\n")
		_T("or, press Enter to cancel waiting.\n")
		);

	HANDLE hThread = (HANDLE)_beginthread(_thread_waitkey, 0, NULL);	

	bool isquit = false;
	HANDLE arhs[2] = { ovlp.hEvent, hThread };
	DWORD waitret = WaitForMultipleObjects(2, arhs, FALSE, INFINITE);
	if(waitret==WAIT_OBJECT_0)
	{
		// todo: see outp values
		
		PrnTs(_T("System asserts Oplock broken.")); // ts
	}
	else if(waitret==WAIT_OBJECT_0+1)
	{
		PrnTs(_T("Got keypress, Cancel waiting the oplock..."));
		succ = CancelIoEx(hfile, &ovlp);
		if(succ)
			PrnTs(_T("Cancel success."));
		else {
			winerr = GetLastError();
			PrnTs(_T("[Unexpect!] CancelIoEx() fail, winerr=%d."), winerr);
		}

		isquit = true;
	}
	else
		assert(0);

	BOOL is_got_broken_notify = GetOverlappedResult(hfile, &ovlp, &retbytes, TRUE);
	if(is_got_broken_notify) {
		PrnTs(_T("FSCTL_REQUEST_OPLOCK ovlp-result: Success."));
	}
	else {
		PrnTs(_T("FSCTL_REQUEST_OPLOCK ovlp-result: Error. WinErr=%d\n"), GetLastError());
	}

	if(!isquit)
	{
		PrnTs(_T("Press Enter to acknowledge lock-breaking."));
		waitret = WaitForSingleObject(hThread, INFINITE);
	}

	if(is_got_broken_notify)
	{
		// We need to respond to system by explicitly acknowledging lock-broken. // No, always got WinErr=301
		//
		ResetEvent(ovlp.hEvent);
		succ = DeviceIoControl(hfile, FSCTL_OPBATCH_ACK_CLOSE_PENDING,
			NULL, 0,
			NULL, 0, 
			&retbytes, &ovlp);
		winerr = GetLastError();
	}

	CloseHandle(ovlp.hEvent);
	// CloseHandle(hThread); // no need for _beginthread()
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	if(argc==1)
	{
		printf("Need a existing filename as parameter.\n");
		exit(4);
	}

	const TCHAR *szfn = argv[1];

	PrnTs(_T("Opening \"%s\""), szfn);

	HANDLE hfile = CreateFile(szfn,
		GENERIC_READ|GENERIC_WRITE, // dwDesiredAccess
		FILE_SHARE_READ|FILE_SHARE_WRITE, // shareMode
		NULL, // no security attribute
		OPEN_EXISTING, // dwCreationDisposition
		FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,
		NULL);
	if(hfile==INVALID_HANDLE_VALUE) {
		_tprintf(_T("File open error. WinErr=%d\n"), GetLastError());
		exit(4);
	}
	
	place_oplock_and_wait_broken(hfile, 3);

	CloseHandle(hfile);

	return 0;
}

