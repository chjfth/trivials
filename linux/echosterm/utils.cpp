#define errno2name_IMPL // <linux/errno2name.h>
#define CircBufws_IMPL
#define termios_util_IMPL
#define CalSpeed_IMPL

#include "echosterm.h"

void ErrExit(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	mm_printf("%w\n", MM_WPAIR_PARAM(fmt, args));
	
	va_end(args);
	exit(1);
}

bool g_isquit;

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

static void quit_handler(int)
{
	// We need this quit handler, so that a Ctrl+C keypress can 
	// tell echo process to quit.
	
	g_isquit = true;
	
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

bool Settle_SIGINT()
{
	struct sigaction act = { };
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = quit_handler; // using SIG_IGN will not let us get EINTR
	
	int err = sigaction(SIGINT, &act, NULL);
	if (err)
		return false;

//	bool succ = BlockSignal(SIGINT, true);
//	if (!succ)
//		return false;
	
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

Uint 
GetOsMillisec32(void)
{
	return (Uint)GetOsMillisec();
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
		' ', //g_is_nonblock ? 'N' : 'B',
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
