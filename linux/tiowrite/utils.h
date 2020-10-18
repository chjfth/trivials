#ifndef __utils_h_
#define __utils_h_

typedef int Fhandle_t;

bool BlockSignal(int sig, bool isblock);

bool prepare_SIGINT();

void report_SIGINT_count();

	
unsigned int ggt_get_mtick(unsigned int sync_boundary=0);
// 'm' implies millisecond. This function has the same idea as Windows GetTickCount.
// if(sync_boundary==1000), this function will wait for 0~999 millisec 
// so that the return value will be xxxx000, xxxx001, xxxx008 etc, for easier
// human observation.

void PrintTss(int sync_boundary, const char *fmt, ...);
void PrintTs(const char *fmt, ...);
void PrintTs100(const char *fmt, ...);

void PrintTsErr(const char *fmt, ...);

void PrintErr(int errno_now, const char *fmt, ...);

void PrintCmdHelp();


extern bool g_is_nonblock;

extern bool g_is_peek_SIGINT;

#endif
