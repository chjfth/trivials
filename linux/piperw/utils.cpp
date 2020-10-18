#include <assert.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>

#include <ps_TypeDecl.h>

#include <commdefs.h>
#include <mm_snprintf.h>
//#include <gadgetlib/timefuncs.h>

#define errno2name_IMPL
#include "utils.h"

bool g_is_peek_SIGINT;

int g_SIGINT_count = 0;

int g_write_seq, g_read_seq;

void reset_rw_seq()
{
	g_write_seq = g_read_seq = 0;
}

int write_ts(int fd, const char *wbuf, int nwr, 
             int *p_msec_use, PrintExtraNewLine_et exline, int *p_errno)
{
	const int seq = ++g_write_seq;
	PrintTs("w%d: write(%d bytes)...", seq, nwr);
	int ret_errno = 0;
	if (!p_errno)
		p_errno = &ret_errno;
	*p_errno = 0;

	__int64 msec1 = SysMillisec64();
	int wret = write(fd, wbuf, nwr);
	*p_errno = errno;
	__int64 msec2 = SysMillisec64();
	
	const char *sztail = exline ? "\n" : "";
	if (wret >= 0)
		PrintTs("w%d: write() returns %d%s", seq, wret, sztail);
	else
	{
		PrintTs("w%d: write() returns %d (errno=%d %s)%s", seq, wret, 
			*p_errno, errno2name(*p_errno), sztail);
		
		if(*p_errno==EAGAIN)
		{
			// verify that poll-for-writable does not fulfill 
			pollfd pollo = {fd, POLLOUT};
			int avai = poll(&pollo, 1, 0); // 0=no wait
			assert(avai==0);
		}
	}

	if (p_msec_use)
		*p_msec_use = (int)(msec2 - msec1);
	
	return wret;
}

int read_ts(int fd, char *rbuf, int nrd, 
            int *p_msec_use, PrintExtraNewLine_et exline, int *p_errno)
{
	const int seq = ++g_read_seq;
	PrintTs("r%d: read(%d bytes)...", seq, nrd);
	int ret_errno = 0;
	if (!p_errno)
		p_errno = &ret_errno;
	*p_errno = 0;
	
	__int64 msec1 = SysMillisec64();
	int rret = read(fd, rbuf, nrd);
	*p_errno = errno;
	__int64 msec2 = SysMillisec64();
	
	const char *sztail = exline ? "\n" : "";
	if (rret >= 0)
		PrintTs("r%d: read() returns %d%s", seq, rret, sztail);
	else
	{
		PrintTs("r%d: read() returns %d (errno=%d)%s", seq, rret, 
			*p_errno, errno2name(*p_errno), sztail);
		
		if(*p_errno==EAGAIN)
		{
			// verify that poll-for-readable does not fulfill 
			pollfd polli = {fd, POLLIN};
			int avai = poll(&polli, 1, 0); // 0=no wait
			assert(avai==0);
		}
	}
	
	if (p_msec_use)
		*p_msec_use = (int)(msec2 - msec1);
	
	return rret;
}

int close_ts(int fd, const char *hint, PrintExtraNewLine_et exline, int *p_errno)
{
	int ret_errno = 0;
	if (!p_errno)
		p_errno = &ret_errno;
	*p_errno = 0;

	PrintTs("# close() %s...", hint);
	
	int ret = close(fd);
	*p_errno = errno;
	
	const char *sztail = exline ? "\n" : "";
	if (ret >= 0)
		PrintTs("# close() returns %d%s", ret, sztail);
	else
	{
		PrintTs("# close() returns %d (errno=%d %s)%s", ret, 
			*p_errno, errno2name(*p_errno), sztail);
	}
	
	return ret;
}

__int64 
SysMillisec64(void)
{
	struct timespec ts;
	if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		return 0; // 0 means fail!
	}
	
	__int64 ret = ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
	return ret == 0 ? 1 : ret;
}

/*
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
*/

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
	pthread_t tid = pthread_self();
	
	// To ensure atomic output, we have to use a tbuf[] buffer.
	// The mm_printf() alone does not guarantee atomic output, i.e. text from different threads
	// may get mixed together in one line.
	char tbuf[1000];

	mm_snprintf(tbuf, sizeof(tbuf), 
		" %c[%u.%03u] %w", 
		tid==g_main_threadid ? ' ' : '*',
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
		PrintTss(0, "%w errno=%d(%s)", MM_WPAIR_PARAM(fmt, args), err, strerror(err));
	
	va_end(args);
}

bool 
set_nonblock_flag(int desc, bool enable)
{
	int flags = fcntl(desc, F_GETFL, 0);
	assert(flags != -1);

	if (enable)
		flags |= O_NONBLOCK;
	else
		flags &= ~O_NONBLOCK;
	
	int err = fcntl(desc, F_SETFL, flags);
	assert(err != -1);
	return true;
}

void PrintPipeBuf(int fd, const char *hint)
{
	errno = 0;
	long bufsz = fpathconf(fd, _PC_PIPE_BUF);
	if (bufsz == -1 && errno == 0) 
	{
		printf("Surprise. There is no size limit for pipe %s-side.\n", hint);
	}
	else if (bufsz == -1 && errno)
	{
		PrintTs("Error querying fpathconf(,_PC_PIPE_BUF) for %s-side.", hint);
	}
	else
	{
		printf("Pipe %s-side _PC_PIPE_BUF size is %ld bytes.\n", hint, bufsz);
	}
}

PipeSize_ret GetPipeSize(int fd, int *p_pipe_size, int *p_atomic_size)
{
	PipeSize_ret ret = PipeAtomic_Error;
	errno = 0;
	long bufsz = fpathconf(fd, _PC_PIPE_BUF);
	if (bufsz == -1 && errno == 0) 
	{
		return PipeAtomic_NoLimit; // that's imposible
	}
	else if (bufsz == -1 && errno)
	{
		return PipeAtomic_Error;
	}
	else
	{
		*p_atomic_size = (int)bufsz;
	}
	
	*p_pipe_size = fcntl(fd, F_GETPIPE_SZ);
	if (*p_pipe_size<=0)
		return PipeAtomic_Error;
	
	return PipeAtomic_HasLimit;
}

bool IsFdWritable(int fd)
{
	pollfd pollo = { fd, POLLOUT };
	int nAvai = poll(&pollo, 1, 0);
	assert(nAvai != -1);
	return nAvai==1 ? true : false;
}


CWorkIOThread::CWorkIOThread(WorkIOs_st &w) : workios(w)
{
	m_hthread = ggt_cxx_thread_create(*this, &CWorkIOThread::thread_start);
	assert(m_hthread);
}

void CWorkIOThread::WaitThreadEnd()
{
	if (m_hthread)
	{
		ggt_simple_thread_waitend(m_hthread);
		m_hthread = NULL;
	}
}

CWorkIOThread::~CWorkIOThread()
{
	WaitThreadEnd();
}

void CWorkIOThread::thread_start()
{
	int nTotWr = 0, nTotRd = 0;
	int i;
	for (i = 0; i < workios.nWorkIO; i++)
	{
		WorkIO_st &wkio = workios.arWorkIO[i];
		
		msleep(wkio.delay_msec);
		
		if (wkio.op == WkioRead)
		{
			wkio.retbytes = read_ts(workios.fd, workios.rbuf + nTotRd, wkio.reqbytes);
			if (wkio.retbytes >= 0)
			{
				nTotRd += wkio.retbytes;
			}
			else
				wkio.ret_errno = errno;
		}
		else if (wkio.op == WkioWrite)
		{
			wkio.retbytes = write_ts(workios.fd, workios.wbuf + nTotWr, wkio.reqbytes);
			if (wkio.retbytes >= 0)
			{
				nTotWr += wkio.retbytes;
			}
			else
				wkio.ret_errno = errno;
		}
		else if (wkio.op == WkioClose)
		{
			wkio.retbytes = close_ts(workios.fd);
			if (wkio.retbytes == -1)
				wkio.ret_errno = errno;
		}
		else if (wkio.op == WkioSignal)
		{
			PrintTs("Sending SIGUSR1 to main thread...\n");
			
			pthread_kill(g_main_threadid, SIGUSR1);
		}
		
	} // for
	
//	printf("CWorkIOThread Delay 1000ms bfr quit.\n");
//	msleep(1000);
//	printf("CWorkIOThread Delay 1000ms done.\n");
}

void Set_SignalHandler(int signum, sighandler_t proc)
{
	struct sigaction act = { };
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = proc;
	
	int err = sigaction(signum, &act, NULL);
	assert(!err);
}

void Fgets(char *s, int size, FILE *stream)
{
	assert(size>0);
	
	s[0] = '\0';
	char *pret = fgets(s, size, stream); 
	(void)pret; // otherwise, gcc blames  ignoring return value of fgets  [-Wunused-result]
	return;
}
