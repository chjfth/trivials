#include <stdio.h>
#include <stdlib.h>
#include <ps_TCHAR.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#else
/* Assume Linux. Compile with:
 g++ -o vaDbgTest -I ../../__chjcxx/common-include/include/linux  -I ../../__chjcxx/common-include/include  vaDbgTest.cpp
*/
#include <unistd.h>
#define _tprintf printf
#define _tmain main
#endif

#define CHHI_ALL_IMPL

#define vaDbgTs_DEBUG1
#define vaDbgTs_DEBUG2
#include <vaDbgTs.h>
//#include "D:\gitw\bookcode-mswin\__chjcxx\common-include\include\vaDbgTs.h"

void do_printf(vaDbg_level_et lvl, const TCHAR *dbgstr, void *ctx)
{
	(void)ctx;
	
	if(lvl>=0)
		_tprintf(_T("<Lv%d>%s"), lvl, dbgstr);
	else
		_tprintf(_T("%s"), dbgstr);
}


int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	_tprintf(_T("Hello, vaDbgTest!\n"));
	
	vaDbg_set_output(do_printf, 0);

//	vaDbgTs_options(vaDbg_seq | vaDbg_newline); // want sequence & HMS only

#ifdef _WIN32
	vaDbgTs_bias_check_interval(2);
#endif

	for(;;)
	{
		vaDbgTs(_T("..."));
#ifdef _WIN32
		Sleep(1000);
#else
		usleep(1000*1000);
#endif
	}

	return 0;
}
