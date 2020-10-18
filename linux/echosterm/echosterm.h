#ifndef __echosterm_h_
#define __echosterm_h_

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>

#include <_MINMAX_.h>
#include <ps_TCHAR.h>
#include <ps_TypeDecl.h>
#include <commdefs.h>
#include <EnsureClnup_common.h>
#include <getopt/sgetopt.h>
#include <mm_snprintf.h>

#include <linux/errno2name.h> // in sdkin
#include <linux/termios_util.h> // in sdkin

#include "CalSpeed.h"
#include "CircBufws.h"

typedef int Fhandle_t;

void ErrExit(const char *fmt, ...);

bool BlockSignal(int sig, bool isblock);

bool Settle_SIGINT();

Uint GetOsMillisec32(void);


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

extern bool g_isquit;

extern bool g_is_nonblock;

extern bool g_is_peek_SIGINT;

#endif
