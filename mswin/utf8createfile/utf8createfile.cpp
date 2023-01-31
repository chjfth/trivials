#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void use_CreateFile(const char * filenam)
{
    printf("Now CreateFile() creating filename: %s\n", filenam);

    HANDLE hfile = CreateFile(filenam,
        GENERIC_READ | GENERIC_WRITE, // to compare with GENERIC_ALL
        FILE_SHARE_READ | FILE_SHARE_WRITE, // shareMode
        NULL, // no security attribute
        CREATE_ALWAYS, // dwCreationDisposition
        0, // FILE_FLAG_DELETE_ON_CLOSE,
        NULL);

    if (hfile == INVALID_HANDLE_VALUE)
    {
        DWORD winerr = GetLastError();
        const int bufsize = 400;
        char szWinErr[bufsize];

        DWORD retchars = FormatMessage(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, winerr,
            0, // LANGID
            szWinErr, bufsize,
            NULL); // A trailing \r\n has been filled.
        if (retchars > 0)
            printf("WinErr=%u: %s\r\n", winerr, szWinErr);
    }
    else
    {
        printf("CreateFile() success.\n");
    }
}

void use_unix_open(const char* filenam)
{
    printf("Now _open() creating filename: %s\n", filenam);

    int hfile = -1;
    errno_t unixerr = _sopen_s(&hfile, filenam, 
        _O_CREAT|_O_RDWR, 
        _SH_DENYNO, // sharing flag
        _S_IWRITE|_S_IREAD);

    if (hfile<0)
    {
        const int bufsize = 400;
        char szerr[bufsize] = "(unknown error)";

        strerror_s(szerr, bufsize, unixerr);
		printf("errno=%u: %s\r\n", unixerr, szerr);
    }
    else
    {
        printf("_open() success.\n");
    }
}


int _tmain(int argc, TCHAR* argv[])
{
	const char *ansi_filenam1 = "AB\xE7\x94\xB5\xE8\x84\x91" "1.txt"; // UTF8: AB电脑1.txt
    const char* ansi_filenam2 = "AB\xE7\x94\xB5\xE8\x84\x91" "2.txt"; // UTF8: AB电脑2.txt

	UINT acp = GetACP();
	if(acp!=65001)
	{
		printf("To run this program, you need to enable UTF8ACP in system-locale.\n");
		exit(1);
	}

    use_CreateFile(ansi_filenam1);

    use_unix_open(ansi_filenam2);

	return 0;
}
