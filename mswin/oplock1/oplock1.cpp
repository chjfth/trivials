#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <assert.h>
#include <conio.h>
#include <process.h>
#include <windows.h>
#include "share.h"

// Sample code according to: 
// https://devblogs.microsoft.com/oldnewthing/20130415-00/?p=4663
// Using opportunistic locks to get out of the way if somebody, April 2013.

// This program shows: When we are using a file(having a file handle opened), 
// we can tell system to notify our code when some other client later tries to
// open the same file.
// This notification is achieved by our putting an oplock on the file handle
// and "wait" for the oplock to be broken. When some other client tries to
// open the same file, Windows system proactively breaks the oplock and notify us.

const TCHAR *g_version = _T("1.2");

void _thread_waitkey(void *)
{
	while(_getch()!='\r');
}

const TCHAR *strOpLockLevel(DWORD level)
{
	static TCHAR sbuf[100] = {};
	sbuf[0] = '\0';
	_tcscat_s(sbuf, level&OPLOCK_LEVEL_CACHE_READ   ? _T("OPLOCK_LEVEL_CACHE_READ|") : _T(""));
	_tcscat_s(sbuf, level&OPLOCK_LEVEL_CACHE_HANDLE ? _T("OPLOCK_LEVEL_CACHE_HANDLE|") : _T(""));
	_tcscat_s(sbuf, level&OPLOCK_LEVEL_CACHE_WRITE  ? _T("OPLOCK_LEVEL_CACHE_WRITE|") : _T(""));

	int slen = (int)_tcslen(sbuf);
	if(slen>0)
		sbuf[slen-1] = '\0'; // delete trailing '|'

	return sbuf;
}

const TCHAR *strObFlags(DWORD flags)
{
	static TCHAR sbuf[100] = {};
	sbuf[0] = '\0';
	_tcscat_s(sbuf, flags&REQUEST_OPLOCK_OUTPUT_FLAG_ACK_REQUIRED ? _T("REQUEST_OPLOCK_OUTPUT_FLAG_ACK_REQUIRED|") : _T(""));
	_tcscat_s(sbuf, flags&REQUEST_OPLOCK_OUTPUT_FLAG_MODES_PROVIDED ? _T("REQUEST_OPLOCK_OUTPUT_FLAG_MODES_PROVIDED|") : _T(""));

	int slen = (int)_tcslen(sbuf);
	if(slen>0)
		sbuf[slen-1] = '\0'; // delete trailing '|'

	return sbuf;
}

void InterpretROPLOutput(const REQUEST_OPLOCK_OUTPUT_BUFFER &ob)
{
	TCHAR szOrigOplockLevel[100] = {};
	_sntprintf_s(szOrigOplockLevel, _TRUNCATE, _T(".OrigOplockLevel = %d (%s)"),
		ob.OriginalOplockLevel, strOpLockLevel(ob.OriginalOplockLevel));

	TCHAR szNewOplockLevel[100] = {};
	_sntprintf_s(szNewOplockLevel, _TRUNCATE, _T(".NewOplockLevel = %d (%s)"),
		ob.NewOplockLevel, strOpLockLevel(ob.NewOplockLevel));

	TCHAR szFlags[100] = {};
	_sntprintf_s(szFlags, _TRUNCATE, _T(".Flags = %d (%s)"), ob.Flags, strObFlags(ob.Flags));

	TCHAR szBreakerInfo[100] = {};
	if(ob.Flags & REQUEST_OPLOCK_OUTPUT_FLAG_MODES_PROVIDED)
	{
		_sntprintf_s(szBreakerInfo, _TRUNCATE, _T("Breaker wants: AccessMode=0x%08X , ShareMode=%d"),
			ob.AccessMode, ob.ShareMode);
	}

	PrnTs(
		_T("REQUEST_OPLOCK_OUTPUT_BUFFER carries info:\n")
		_T("    %s\n")
		_T("    %s\n")
		_T("    %s\n")
		_T("    %s")
		,
		szOrigOplockLevel,
		szNewOplockLevel,
		szFlags,
		szBreakerInfo
		);
}

typedef DWORD WinErr_t;

WinErr_t 
ioctl_FSCTL_REQUEST_OPLOCK(HANDLE hfile, DWORD req_oplock_level, DWORD flags,
	REQUEST_OPLOCK_OUTPUT_BUFFER *outpp, OVERLAPPED *povlp)
{
	// flags: REQUEST_OPLOCK_INPUT_FLAG_REQUEST or REQUEST_OPLOCK_INPUT_FLAG_ACK

	REQUEST_OPLOCK_INPUT_BUFFER inp =
	{
		REQUEST_OPLOCK_CURRENT_VERSION, // 1
		sizeof(inp),
		req_oplock_level,
		flags
	};

	//REQUEST_OPLOCK_OUTPUT_BUFFER outp = { REQUEST_OPLOCK_CURRENT_VERSION, sizeof(outp) };
	memset(outpp, 0, sizeof(*outpp));
	outpp->StructureVersion = REQUEST_OPLOCK_CURRENT_VERSION;
	outpp->StructureLength = sizeof(*outpp);

	SetLastError(0);
	DWORD retbytes = 0;
	BOOL succ = DeviceIoControl(hfile, FSCTL_REQUEST_OPLOCK,
		&inp, sizeof(inp),
		outpp, sizeof(*outpp), 
		&retbytes, povlp);
	if(succ)
		return NOERROR;
	else
		return GetLastError();
}

void place_oplock_and_wait_broken(HANDLE hfile, DWORD oplock_level)
{
	DWORD succ = 0;
	OVERLAPPED ovlp = {};
	ovlp.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	PrnTs(_T("Issuing FSCTL_REQUEST_OPLOCK on the file handle(0x%08X)."), hfile);

	REQUEST_OPLOCK_OUTPUT_BUFFER outp = {};
	DWORD retbytes = 0;
	DWORD winerr = ioctl_FSCTL_REQUEST_OPLOCK(hfile, 
		oplock_level, REQUEST_OPLOCK_INPUT_FLAG_REQUEST,
		&outp, &ovlp);
	if(winerr!=ERROR_IO_PENDING)
	{
		PrnTs(_T("[UNEXPECT!] Got WinErr=%d, should be ERROR_IO_PENDING."), winerr);
		exit(4);
	}

	PrnTs(_T("OK. We have placed an oplock on the file handle.\n")
		_T("    Now waiting for the lock to be broken.\n")
		_T("    Please Use another program to access the same file to see the oplock broken,\n")
		_T("    or, press Enter to cancel waiting.\n")
		);

	HANDLE hThread = (HANDLE)_beginthread(_thread_waitkey, 0, NULL);	

	bool is_active_quit = false;
	HANDLE arhs[2] = { ovlp.hEvent, hThread };
	DWORD waitret = WaitForMultipleObjects(2, arhs, FALSE, INFINITE);
	if(waitret==WAIT_OBJECT_0)
	{
		PrnTs(_T("FSCTL_REQUEST_OPLOCK ovlp-operation completed."));
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

		is_active_quit = true;
	}
	else
		assert(0);

	BOOL is_got_broken_request = GetOverlappedResult(hfile, &ovlp, &retbytes, TRUE);
	if(is_got_broken_request) {
		PrnTs(_T("FSCTL_REQUEST_OPLOCK completion-result: Success. (=Some other process wants to break the lock.)"));
		InterpretROPLOutput(outp);
	}
	else {
		PrnTs(_T("FSCTL_REQUEST_OPLOCK completion-result: Fail. WinErr=%d\n"), GetLastError());
	}

	bool need_ack = (is_got_broken_request 
		&& (outp.Flags & REQUEST_OPLOCK_OUTPUT_FLAG_ACK_REQUIRED));

	if(!is_active_quit)
	{
		if(need_ack)
		{
			PrnTs(_T("Press Enter to acknowledge(=agree) lock-breaking."));
		}
		else
		{
			PrnTs(_T("Press Enter to end this program."));
		}
		
		waitret = WaitForSingleObject(hThread, INFINITE);
	}

	if(need_ack)
	{
		PrnTs(_T("Now issue ioctl: REQUEST_OPLOCK_INPUT_FLAG_ACK ..."));
		
		ResetEvent(ovlp.hEvent);
		winerr = ioctl_FSCTL_REQUEST_OPLOCK(hfile, 
			oplock_level, REQUEST_OPLOCK_INPUT_FLAG_ACK,
			&outp, &ovlp);
		if(winerr==ERROR_IO_PENDING)
		{
			succ = GetOverlappedResult(hfile, &ovlp, &retbytes, TRUE);
			winerr = GetLastError();
		}
	
		if(succ)
			PrnTs(_T("REQUEST_OPLOCK_INPUT_FLAG_ACK success."));
		else
			PrnTs(_T("REQUEST_OPLOCK_INPUT_FLAG_ACK fail, WinErr=%d."), winerr);
	}

	CloseHandle(ovlp.hEvent);
	// CloseHandle(hThread); // no need for _beginthread()
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");

	_tprintf(_T("OpLock1 version %s, demo of how opportunistic-lock works.\n"), g_version);
	
	if(argc==1)
	{
		_tprintf(_T("Need an existing filename as parameter.\n"));
		exit(4);
	}

	const TCHAR *szfn = argv[1];

	PrnTs(_T("Opening \"%s\""), szfn);

	HANDLE hfile = CreateFile(szfn,
		GENERIC_READ|GENERIC_WRITE, // dwDesiredAccess
		FILE_SHARE_READ|FILE_SHARE_WRITE, // shareMode
		NULL, // no security attribute
		OPEN_EXISTING, // dwCreationDisposition
		FILE_FLAG_OVERLAPPED,
		NULL);
	if(hfile==INVALID_HANDLE_VALUE) {
		_tprintf(_T("File open error. WinErr=%d.\n"), GetLastError());
		exit(4);
	}
	
	place_oplock_and_wait_broken(hfile, 3);

	PrnTs(_T("Calling Closehandle()..."));
	DWORD succ = CloseHandle(hfile);
	if(succ)
		PrnTs(_T("Success Closehandle()."));
	else
		PrnTs(_T("CloseHandle() fail, WinErr=%d"), GetLastError());

	return 0;
}
