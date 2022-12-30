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

	PrnTs(_T("OK. We have placed an oplock on the file handle.\n")
		_T("    Now waiting for the lock to be broken.\n")
		_T("    Please Use another program to access the same file to see the oplock broken,\n")
		_T("    or, press Enter to cancel waiting.\n")
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
		FILE_FLAG_OVERLAPPED,
		NULL);
	if(hfile==INVALID_HANDLE_VALUE) {
		_tprintf(_T("File open error. WinErr=%d.\n"), GetLastError());
		exit(4);
	}
	
	place_oplock_and_wait_broken(hfile, 3);

	PrnTs(_T("Calling Closehandle()..."));
	CloseHandle(hfile);
	PrnTs(_T("Done    Closehandle()."));

	return 0;
}
