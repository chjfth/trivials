#include <assert.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdarg.h>
#include <termios.h>
#include <ps_TypeDecl.h>

#include <commdefs.h>
#include <mm_snprintf.h>

#define errno2name_IMPL 
#include <linux/errno2name.h>
//
#define termios_util_IMPL
#include <linux/termios_util.h>

#include "utils.h"

bool g_is_peek_SIGINT;

int g_SIGINT_count = 0;

bool BlockSignal(int sig, bool isblock)
{
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGINT);
	
	int err = sigprocmask(
		isblock ? SIG_BLOCK : SIG_UNBLOCK,
		&sigset,
		NULL);
	return err ? false : true;
}

static void null_handler(int)
{
	// We need this null handler, so that a Ctrl+C keypress can 
	// break the in-progress write().
	
	g_SIGINT_count++;
	
	if (g_is_peek_SIGINT)
	{
		printf("! SIGINT(count=%d)\n", g_SIGINT_count);
	}
}

void report_SIGINT_count()
{
	if (g_SIGINT_count > 0)
		mm_printf("tiowrite info: SIGINT is caught %d times.\n", g_SIGINT_count);
}

bool prepare_SIGINT()
{
	struct sigaction act = { };
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = null_handler; //SIG_IGN;
	
	int err = sigaction(SIGINT, &act, NULL);
	if (err)
		return false;

	bool succ = BlockSignal(SIGINT, true);
	if (!succ)
		return false;
	
	return true;
}

__int64 
GetOsMillisec(void)
{
	struct timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		return 0; // 0 means fail!
	}
	
	__int64 ret = ts.tv_sec*1000 + ts.tv_nsec/1000000;
	return ret==0 ? 1 : ret;
}

unsigned int 
ggt_get_mtick(unsigned int sync_boundary)
{
	Uint mtick = (Uint)GetOsMillisec();
	if (sync_boundary == 0)
		return mtick;
	
	Uint mboundary = UP_ROUND(mtick, sync_boundary);

	if (mboundary > mtick)
	{
		// Wait time-elapse until time boundary arrives
		for(; ;)
		{
			usleep(1000); // sleep 1 millisec

			mtick = (Uint)GetOsMillisec();
			if (mtick >= mboundary)
				break;
		}
	}

	return mtick;
}

void PrintTssv(int sync_boundary, const char *fmt, va_list &args)
{
	Uint mtick = ggt_get_mtick(sync_boundary);
	
	// To ensure atomic output, we have to use a tbuf[] buffer.
	// The mm_printf() alone does not guarantee atomic output, i.e. text from different threads
	// may get mixed together in one line.
	char tbuf[1000];

	mm_snprintf(tbuf, sizeof(tbuf), 
		" %c[%u.%03u] %w", 
		g_is_nonblock ? 'N' : 'B',
		mtick/1000, mtick%1000,
		MM_WPAIR_PARAM(fmt, args));

	printf("%s\n", tbuf);
}

void PrintTss(int sync_boundary, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	PrintTssv(sync_boundary, fmt, args);
	va_end(args);
}
void PrintTs(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	PrintTssv(0, fmt, args);
	va_end(args);
}
void PrintTs100(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	PrintTssv(100, fmt, args);
	va_end(args);
}

void PrintTsErr(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	int err = errno;
	
	if (!err)
		PrintTssv(0, fmt, args);
	else 
		PrintTss(0, "%w errno=%d(%s)", MM_WPAIR_PARAM(fmt, args), err, errno2name(err));
	
	va_end(args);
}

void PrintErr(int errno_now, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	if (!errno_now)
		mm_printf("%w\n", MM_WPAIR_PARAM(fmt, args));
	else 
	{
		mm_printf("%w errno=%d(%s, %s)\n", MM_WPAIR_PARAM(fmt, args), 
		          errno_now, errno2name(errno_now), strerror(errno_now));
	}
	
	va_end(args);
}


void PrintCmdHelp()
{
	printf("\n");
	mm_printf("Available operations (needs Enter):\n");
	mm_printf(" w : Just call write() to write to file handle\n");
	mm_printf(" s : select() for writable then write()\n");
	mm_printf(" p : poll() for writable then write()\n");
	mm_printf(" t : Show total bytes written\n");
	mm_printf(" r : read() for some bytes()\n");
	mm_printf(" q : Quit\n");
	mm_printf(" 4000 : change write/read bytes to 4000\n");
	mm_printf(" <Enter> : Execute previous operation again\n");
	mm_printf("\n");
	
}
