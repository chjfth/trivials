/*
Using VC2010+ x64 to compile this program, we can see .pdata section generated
in the EXE(PE+ format), and UNWIND_INFO, UNWIND_CODE structures for each function
in the generated x64stkframe.cod file.

	cl /FAsc /Zi /RTC1 x64stkframe.cpp

Note of the /RTC1 option: 
* Without /RTC1, Bottom() will have only one unwind-code.
* With /RTC1, Bottom() will have two unwind-codes.

That difference can be seen at $unwind$Bottom in x64stkframe.cod .

*/

#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

extern"C" void Middle(int c, int d);
extern"C" void Bottom(int e);

extern"C" void Top(int a, int b)
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

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");

    Top(0x41414141, 0x42424242);
   
    printf("Hello, x64stkframe!\n");
    return 0;
}
