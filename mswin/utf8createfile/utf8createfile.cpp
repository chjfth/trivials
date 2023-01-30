#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

int _tmain(int argc, TCHAR* argv[])
{
	const char *ansi_filenam = "AB\xE7\x94\xB5\xE8\x84\x91.txt"; // UTF8: AB电脑

	UINT acp = GetACP();
	if(acp!=65001)
	{
		printf("To run this program, you need to enable UTF8ACP in system-locale.\n");
		exit(1);
	}
	
	printf("Now CreateFile() creating filename: %s\n", ansi_filenam);

    HANDLE hfile = CreateFile(ansi_filenam,
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
        printf("Create success.\n");
    }

	return 0;
}
