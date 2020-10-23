/*
	Should be 64-bit capable.
*/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>

//#include <tchar.h>
	typedef char TCHAR;
	#define _T(str) str
	#define _tprintf printf
	#define _tcscmp strcmp
	#define _tcslen strlen
	typedef int HANDLE;

#include <locale.h>

typedef int RE_CODE;
#define NOERROR_0 0
#define GetEleQuan(array) ( sizeof(array)/sizeof(array[0]) )
#define GetEleQuan_i(array) ((int)GetEleQuan(array))

// Exit value defines:
#define EXIT_SUCCESS 0
#define EXIT_INVALID_OPTION 1
#define EXIT_FILE_OPEN_ERROR 2
#define EXIT_GET_SIZE_ERROR 3
#define EXIT_SET_SIZE_ERROR 4


HANDLE 
hpOpenFile(const TCHAR szfn[]) // hp: helper
{
	int h = open64(szfn, O_RDWR);
		// Yes, you'd better use open64() instead of open().
		// On VMware ESX 4.1U1, with open(), you will not be able to change
		// file size beyond 2GB, and you get errno 22(EINVAL).
	
	if(h<0)
	{
		_tprintf(_T("Open file error! errno(%d)\n"), errno);
		exit(EXIT_FILE_OPEN_ERROR);
	}
	return h;
}

__int64 
hpGetFileSize(HANDLE h)
{
	__int64 size = lseek64(h, 0, SEEK_END);
	
	if(size==-1)
	{
		_tprintf(_T("Get file size error! errno(%d)\n"), errno);
		exit(EXIT_GET_SIZE_ERROR);
	}
	
	return size;
}

RE_CODE 
hpSetFileSize(HANDLE h, __int64 NewSize)
{
	int err = ftruncate64(h, NewSize);

	if(err)
	{
//		_tprintf(_T("Set file size(%lld) error! errno(%d)\n"), NewSize, errno);
		exit(EXIT_SET_SIZE_ERROR);
	}
	
	return NOERROR_0;
}

__int64 
CalNewSize(__int64 OldSize, const TCHAR szUserHint[])
{
	enum { SizeSet=0, SizeInc=1, SizeDec=-1 };
	int proc = SizeSet;
	if(szUserHint[0]==_T('+'))
		proc = SizeInc;
	else if(szUserHint[0]==_T('-'))
		proc = SizeDec;

	__int64 iUserInput = strtoll(szUserHint, NULL, 10);

	__int64 NewSize;
	if(proc==SizeSet)
		NewSize = iUserInput;
	else
		NewSize = OldSize + iUserInput;

	return NewSize;
}

void 
DoInteractive(const TCHAR szfn[])
{
	TCHAR buf[256];

	HANDLE h = hpOpenFile(szfn);
	__int64 OldSize = hpGetFileSize(h);
	
	_tprintf(_T("Current file size: %I64d\n"), OldSize);
	_tprintf(_T("   Input new size: "));
	
	fgets(buf, GetEleQuan_i(buf), stdin);

	int ulen = _tcslen(buf);
	if(buf[ulen-1]=='\r' || buf[ulen-1]=='\n')
		ulen--;
	if(buf[ulen-1]=='\r' || buf[ulen-1]=='\n')
		ulen--;
	buf[ulen] = '\0';

	__int64 NewSize = CalNewSize(OldSize, buf);

	_tprintf(_T("  New size set to: %I64d, right(y/n)?"), NewSize);
	fgets(buf, GetEleQuan_i(buf), stdin);
	if(buf[0]!='y' && buf[1]!='Y')
	{
		_tprintf(_T("Nothing Done.\n"));
		exit(EXIT_SUCCESS);
	}	

	hpSetFileSize(h, NewSize);

	_tprintf(_T("Set file size done.\n"));

	close(h);
}

void 
DoNonInteractive(const TCHAR szfn[], const TCHAR szUserHint[])
{
	HANDLE h = hpOpenFile(szfn);
	__int64 OldSize = hpGetFileSize(h);
	
	__int64 NewSize = CalNewSize(OldSize, szUserHint);

	hpSetFileSize(h, NewSize);

	close(h);
}

void 
PrintUsageAndQuit()
{
	_tprintf(_T("Usage:\n"));
	_tprintf(_T("Interactive use:\n"));
	_tprintf(_T("    ichsize -i <filepath>\n"));
	_tprintf(_T("Non-interactive use:\n"));
	_tprintf(_T("    ichsize -n <filepath> <newsize or +/-size>\n"));
	_tprintf(_T("    Example:\n"));
	_tprintf(_T("        Change file size to 456001536: ichsize -n c:/a.iso 456001536\n"));
	_tprintf(_T("        Increase 2048 bytes: ichsize -n c:\\a.iso +2048\n"));
	_tprintf(_T("        Decrease 2048 bytes: ichsize -n c:\\a.iso -2048\n"));
	exit(EXIT_INVALID_OPTION);
}

int main(int argc, TCHAR *argv[])
{
	setlocale(LC_ALL, "");

	printf("Compiled on %s %s\n", __DATE__,__TIME__);

	if(argc<2)
		PrintUsageAndQuit();

	int is_iuse = _tcscmp(argv[1], _T("-i"))==0;
	int is_niuse = _tcscmp(argv[1], _T("-n"))==0;
	if(is_iuse)
	{
		if(argc!=3)
			PrintUsageAndQuit();

		DoInteractive(argv[2]);
	}
	else if(is_niuse)
	{
		if(argc!=4)
			PrintUsageAndQuit();

		DoNonInteractive(argv[2], argv[3]);
	}
	else
	{
		_tprintf(_T("Invalid option. Neither -i or -n is assigned.\n"));
		PrintUsageAndQuit();
	}
	
	return EXIT_SUCCESS;
}
