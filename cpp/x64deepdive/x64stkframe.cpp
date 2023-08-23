#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>

void Middle(int c, int d);
void Bottom(int e);

void Top(int a, int b)
{
    int toplocal = b + 5;
    Middle(a, toplocal);
}

void Middle(int c, int d)
{
    Bottom(c + d);
}

void Bottom(int e)
{
    int btlocal1 = 0x61616161;
    int btlocal2 = 0x62626262;
    printf("In Bottom() e=%X ; %X , %X\n", e, btlocal1, btlocal2);
}

int _tmain(int argc, TCHAR* argv[])
{
    setlocale(LC_ALL, "");

    Top(0x41414141, 0x42424242);
   
    _tprintf(_T("Hello, x64stkframe!\n"));
    _tprintf(_T("sizeof(TCHAR)=%d\n"), (int)sizeof(TCHAR));
    return 0;
}
