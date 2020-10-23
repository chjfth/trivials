#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>

#include <ps_TCHAR.h>
#include <ps_TypeDecl.h>
#include <commdefs.h>
#include <EnsureClnup_common.h>
//#include <getopt/sgetopt.h>
#include <mm_snprintf.h>

//#include "fd_select.h"
#include "utils.h"

#define MSEC_INDEF_WAIT 2000 
#define MSEC_SHORT_WAIT 200

#define BIGWRITE_EXTRA 20

pthread_t g_main_threadid;

MakeCleanupClass(Cec_LibcFh, int, close, int, -1)

int g_sigpipe_count = 0;

static void null_handler(int)
{
}
static void Sigpipe_handler(int)
{
	g_sigpipe_count++;
}


enum ReadBlocking_et  { Rblocking=0, Rnonb=1 };
enum WriteBlocking_et { Wblocking=0, Wnonb=1 };

enum WhichSide_et { ReadSide=0, WriteSide=1 };

struct PipePair_st
{
	int fdr, fdw;
	
	PipePair_st()
	{
		int fdpair[2];
		int err = pipe(fdpair);
		assert(!err);
		fdr = fdpair[0]; fdw = fdpair[1];		
	}
	
	~PipePair_st()
	{
		CloseRead(NoExLine);
		CloseWrite(NoExLine);
	}
	
	void SetBlockingMode(ReadBlocking_et rbm, WriteBlocking_et wbm)
	{
		set_nonblock_flag(fdr, rbm == Rnonb ? true : false);
		set_nonblock_flag(fdw, wbm == Wnonb ? true : false);
		
		printf("Write-side(fd=%d): %s , Read-side(fd=%d): %s\n",
			fdw , wbm == Wnonb ? "Nonblocking" : "Blocking",
			fdr , rbm == Rnonb ? "Nonblocking" : "Blocking"
			);
	}
	
	int CloseRead(PrintExtraNewLine_et ex=ExLine)
	{
		int ret = 0;
		if (fdr != -1)
			ret = close_ts(fdr, "read-side", ex);
		fdr = -1;
		return ret;
	}
	int CloseWrite(PrintExtraNewLine_et ex=ExLine)
	{
		int ret = 0;
		if (fdw != -1)
			ret = close_ts(fdw, "write-side", ex);
		fdw = -1;
		return ret;
	}
};

void wLess_rMore_RB()
{	
	// Action: Write less, read more.
	// Verify: Whether read() blocks for full data. Expect No.
	
	PipePair_st pp;
	pp.SetBlockingMode(Rblocking, Wblocking);
	
	char wbuf[10], rbuf[10];
	const int nwr = 2, nrd = 10;
	
	int wret = write_ts(pp.fdw, wbuf, nwr);
	assert(wret == nwr);
	
	int rret = read_ts(pp.fdr, rbuf, nrd);
	assert(rret == nwr);
	
	// Verify that going on read() will block indefinitely. 
	// To avoid blocking forever, I'll create a thread in advance to 
	// inform him to quit after waiting for two seconds.
	//
	WorkIO_st wkio = { MSEC_INDEF_WAIT, WkioWrite, 1 };
	WorkIOs_st workios = { pp.fdw, wbuf, NULL, &wkio, 1 };
	CWorkIOThread iothread(workios);
	//
	int msec_use = 0;
	rret = read_ts(pp.fdr, rbuf, 1, &msec_use);
	assert(rret == 1);
	assert_longer_than(msec_use, MSEC_INDEF_WAIT);
}

void wLess_rMore_RNB()
{
	// Verify: nonblocking reading an empty pipe will get EAGAIN.
	
	PipePair_st pp;
	pp.SetBlockingMode(Rnonb, Wblocking);
	
	char wbuf[10], rbuf[10];
	const int nwr = 2, nrd = 10;
	
	int wret = write_ts(pp.fdw, wbuf, nwr);
	assert(wret == nwr);
	
	int rret = read_ts(pp.fdr, rbuf, nrd);
	assert(rret == nwr);
	
	rret = read_ts(pp.fdr, rbuf, 1);
	if (rret == -1)
		PrintTsErr("Nonblock-reading an empty pipe:");
	
	assert(rret == -1 && errno == EAGAIN);
}

void wWrite_wClose_r3times(ReadBlocking_et rbm)
{
	PipePair_st pp;
	pp.SetBlockingMode(rbm, Wblocking);
	
	char wbuf[10], rbuf[10];
	const int nwr = 2, nrd = 10;
	
	int wret = write_ts(pp.fdw, wbuf, nwr);
	assert(wret == nwr);
	
	pp.CloseWrite();
	
	int rret = read_ts(pp.fdr, rbuf, nrd);
	assert(rret == nwr);
	
	msleep(100);
	
	rret = read_ts(pp.fdr, rbuf, nrd);
	assert(rret == 0);
	rret = read_ts(pp.fdr, rbuf, nrd);
	assert(rret == 0);
}

void wWrite_wClose_r3times_RB()
{
	wWrite_wClose_r3times(Rblocking);
}

void wWrite_wClose_r3times_RNB()
{
	wWrite_wClose_r3times(Rnonb);
}

void QueryPrintPipesize(int fdpipe, int &pipesize, int &atomic_size)
{
	PipeSize_ret psret = GetPipeSize(fdpipe, &pipesize, &atomic_size);
	assert(psret == PipeAtomic_HasLimit);
	printf("  Pipe-size %d, atomic-write-size %d (diff=%d).\n", 
		pipesize, atomic_size, pipesize-atomic_size);
}

void wWait_Atomic_WB()
{
	// When
	// * bytes to write(n) <= PIPE_BUF
	// * n > v (v is pipe current vacant bytes)
	// The write will block until the vacant bytes goes up to at least n.
	
	PipePair_st pp;
	pp.SetBlockingMode(Rblocking, Wblocking);
	
	int pipesize = 0, atomic_size = 0;
	QueryPrintPipesize(pp.fdw, pipesize, atomic_size);
	
	// alloc a buffer that is equal to pipesize+atomic_size.
	Cec_delete_pchar cec_wbuf = new char[pipesize+atomic_size];
	const char *pwbuf = (char *)cec_wbuf;
	const char *pwbuf_end_ = pwbuf + pipesize+atomic_size;
	
	const int vacant1 = 100;
	assert(pipesize > vacant1);
	
	int nwr1 = pipesize - vacant1;
	printf("Write %d bytes to make pipe %d bytes(<PIPE_BUF) vacant.\n", nwr1, vacant1);
	int wret = write_ts(pp.fdw, pwbuf, nwr1);
	assert(wret == nwr1);
	
	Cec_delete_pchar cec_rbuf = new char[atomic_size];
	char *prbuf = (char *)cec_rbuf;
	WorkIO_st ar_wkio[] = {
		{ MSEC_INDEF_WAIT, WkioRead, atomic_size-vacant1 }, // pull 1, pipe will be atomic-byte vacant
		{ MSEC_INDEF_WAIT, WkioRead, vacant1 }, // pull 2
	};
	WorkIOs_st workios = { pp.fdr, NULL, prbuf, ar_wkio, GetEleQuan(ar_wkio) };
	CWorkIOThread iothread(workios);
	//
	int msec_use = 0;
	printf("Write %d more bytes, expect to block...\n", atomic_size);
	wret = write_ts(pp.fdw, pwbuf+nwr1, atomic_size, &msec_use);
	assert(wret == atomic_size);

	if (IsLongerThan(msec_use, MSEC_INDEF_WAIT * 2))
	{
		// openSUSE Linux 13.1, 42.1 exhibit this behavior.
		printf("(LAZY atomic write detected)\n"); 
	}
	else if (IsLongerThan(msec_use, MSEC_INDEF_WAIT))
	{
		printf("(HUSTLE atomic write detected)\n");
	}
	else
	{
		printf("!!! Weird return bytes from write(), return=%d.\n", wret);
		assert(0);
	}
		
}

void wAtomic_SignalBreak_w0()
{
	// When
	// * bytes to write(n) <= PIPE_BUF
	// * n > v (v is pipe current vacant bytes)
	// The write will block, then we break it with a signal to see 0 bytes written.
	
	PipePair_st pp;
	pp.SetBlockingMode(Rblocking, Wblocking);
	
	int pipesize = 0, atomic_size = 0;
	QueryPrintPipesize(pp.fdw, pipesize, atomic_size);
	
	// alloc a buffer that is equal to pipesize+atomic_size. 
	// (although a bit large than actually required)
	Cec_delete_pchar cec_wbuf = new char[(pipesize+atomic_size)];
	const char *pwbuf = (char *)cec_wbuf;
	const char *pwbuf_end_ = pwbuf + (pipesize+atomic_size);
	
	const int vacant1 = 100;
	assert(pipesize > vacant1);
	
	int nwr = pipesize - vacant1;
	printf("Write %d bytes to make pipe %d bytes(<PIPE_BUF) vacant.\n", nwr, vacant1);
	int wret = write_ts(pp.fdw, pwbuf, nwr);
	assert(wret == nwr);
	
	Cec_delete_pchar cec_rbuf = new char[pipesize];
	char *prbuf = (char *)cec_rbuf;
	WorkIO_st ar_wkio [2] = {
		{MSEC_INDEF_WAIT, WkioSignal},
		{MSEC_SHORT_WAIT, WkioRead, pipesize}, // to verify in-pipe bytes
	};
	WorkIOs_st workios = {pp.fdr, NULL, prbuf, ar_wkio, GetEleQuan(ar_wkio)};
	CWorkIOThread iothread(workios);
	//
	int msec_use = 0;
	printf("Write %d more bytes, expect to block...\n", atomic_size);
	wret = write_ts(pp.fdw, pwbuf + nwr, atomic_size, &msec_use);
	assert(wret == -1);
	assert(errno == EINTR);
	assert_longer_than(msec_use, MSEC_INDEF_WAIT);
	
	iothread.WaitThreadEnd();
	
	assert(ar_wkio[1].retbytes == nwr); // this can deduce that 0 byte was written
	
	// Q: any more direct way to query in-pipe bytes?
}

void NearFull_wAtomic_EAGAIN_WNB()
{
	// When
	// * bytes to write(n) <= PIPE_BUF
	// * n > v (v is pipe current vacant bytes)
	// The non-blocking write fails with EAGAIN.
	
	PipePair_st pp;
	pp.SetBlockingMode(Rblocking, Wnonb);
	
	int pipesize = 0, atomic_size = 0;
	QueryPrintPipesize(pp.fdw, pipesize, atomic_size);
	
	// alloc a buffer that is equal to pipesize+atomic_size. 
	// (although a bit large than actually required)
	Cec_delete_pchar cec_wbuf = new char[(pipesize+atomic_size)];
	const char *pwbuf = (char *)cec_wbuf;
//	const char *pwbuf_end_ = pwbuf + (pipesize+atomic_size);
	
	const int vacant1 = 100;
	assert(pipesize > vacant1);
	
	int nwr = pipesize - vacant1;
	printf("Write %d bytes to make pipe %d bytes(<PIPE_BUF) vacant.\n", nwr, vacant1);
	int wret = write_ts(pp.fdw, pwbuf, nwr);
	assert(wret == nwr);

	// Write again will fail with EAGAIN.
	int ret_errno = 0, msec_use = 0;
	wret = write_ts(pp.fdw, pwbuf+nwr, atomic_size, &msec_use, ExLine, &ret_errno);
	assert(wret == -1 && ret_errno == EAGAIN);
	assert_shorter_than(msec_use, 0);
}

void wNonAtomic_SignalBreak_FL()
{
	// When
	// * bytes to write(n) > PIPE_BUF
	// * n > v (v is pipe current vacant bytes)
	// The write will block. 
	// When later get a signal, write() may report partial bytes written.
	// FL means flow_size is less than atomic_size. See LAZY or HUSTLE.
	
	PipePair_st pp;
	pp.SetBlockingMode(Rblocking, Wblocking);
	
	int pipesize = 0, atomic_size = 0;
	QueryPrintPipesize(pp.fdw, pipesize, atomic_size);
	
	const int atomic2x = atomic_size * 2;
	const int wbuf_total = pipesize + atomic2x;
	// alloc a buffer that is equal to wbuf_total. 
	Cec_delete_pchar cec_wbuf = new char[wbuf_total];
	const char *pwbuf = (char *)cec_wbuf;
	const char *pwbuf_end_ = pwbuf + (pipesize + wbuf_total);
	
	const int flow_size = 100;
	
	int nwr1 = pipesize;
	printf("Write %d bytes to make pipe full.\n", pipesize);
	int wret = write_ts(pp.fdw, pwbuf, nwr1);
	assert(wret == nwr1);
	
	Cec_delete_pchar cec_rbuf = new char[wbuf_total];
	char *prbuf = (char *)cec_rbuf;
	WorkIO_st ar_wkio[] = {
		{MSEC_INDEF_WAIT, WkioRead, flow_size},
		{MSEC_INDEF_WAIT, WkioSignal},
		{MSEC_SHORT_WAIT, WkioRead, wbuf_total-flow_size},
	};
	WorkIOs_st workios = {pp.fdr, NULL, prbuf, ar_wkio, GetEleQuan(ar_wkio)};
	CWorkIOThread iothread(workios);
	//
	int msec_use = 0;
	int nwr2 = atomic2x;
	printf("Write %d more bytes(>PIPE_BUF), expect to block...\n", nwr2);
	wret = write_ts(pp.fdw, pwbuf+nwr1, nwr2, &msec_use);
	assert_longer_than(msec_use, MSEC_INDEF_WAIT*2);
	if (wret == flow_size)
	{
		printf("(NonAtomic write HUSTLE behavior detected.)\n");
	}
	else if (wret == -1)
	{
		// This is openSUSE linux behavior.
		assert(errno == EINTR);
		printf("(NonAtomic write LAZY behavior detected.)\n");
		
		iothread.WaitThreadEnd();
		assert(ar_wkio[2].retbytes == pipesize - flow_size);
	}
	else
	{
		printf("!!! Weird return bytes from write(), return=%d.\n", wret);
		assert(0);	
	}
}

void wNonAtomic_SignalBreak_FM()
{
	// When
	// * bytes to write(n) > PIPE_BUF
	// * n > v (v is pipe current vacant bytes)
	// The write will block. 
	// When later get a signal, write() may report partial bytes written.
	//
	// FM means flow_size is more than atomic_size. 
	// See whether partial written bytes is multiple of atomic_size.
	
	PipePair_st pp;
	pp.SetBlockingMode(Rblocking, Wblocking);
	
	int pipesize = 0, atomic_size = 0;
	QueryPrintPipesize(pp.fdw, pipesize, atomic_size);
	
	const int atomic2x = atomic_size * 2;
	const int wbuf_total = pipesize + atomic2x;
	// alloc a buffer that is equal to wbuf_total. 
	Cec_delete_pchar cec_wbuf = new char[wbuf_total];
	const char *pwbuf = (char *)cec_wbuf;
	const char *pwbuf_end_ = pwbuf + (pipesize + wbuf_total);
	
	const int flow_size = atomic_size + 100; // diff!
	
	int nwr1 = pipesize;
	printf("Write %d bytes to make pipe full.\n", pipesize);
	int wret = write_ts(pp.fdw, pwbuf, nwr1);
	assert(wret == nwr1);
	
	Cec_delete_pchar cec_rbuf = new char[wbuf_total];
	char *prbuf = (char *)cec_rbuf;
	WorkIO_st ar_wkio [] = {
		{MSEC_INDEF_WAIT, WkioRead, flow_size},
		{MSEC_INDEF_WAIT, WkioSignal},
		{MSEC_SHORT_WAIT, WkioRead, wbuf_total-flow_size},
	};
	WorkIOs_st workios = {pp.fdr, NULL, prbuf, ar_wkio, GetEleQuan(ar_wkio)};
	CWorkIOThread iothread(workios);
	//
	int msec_use = 0;
	int nwr2 = atomic2x;
	printf("Write %d more bytes(>PIPE_BUF), expect to block...\n", nwr2);
	wret = write_ts(pp.fdw, pwbuf+nwr1, nwr2, &msec_use);
	assert_longer_than(msec_use, MSEC_INDEF_WAIT * 2);
	if (wret == flow_size)
	{
		printf("(NonAtomic write EAGER behavior detected.)\n");
	}
	else if (wret == atomic_size)
	{
		// This is openSUSE linux behavior. Only pipesize+atomic_size written so far.
		printf("(NonAtomic write BOUNDARY behavior detected.)\n");

		iothread.WaitThreadEnd();
		assert(ar_wkio[0].retbytes == flow_size);
		assert(ar_wkio[2].retbytes == pipesize+atomic_size-flow_size);
	}
	else
	{
		printf("!!! Weird return bytes from write(), return=%d.\n", wret);
		assert(0);	
	}
}

void PipeFull_wNonAtomic_EAGAIN()
{
	// When pipe is full(0 byte vacant), 
	// write >PIPE_BUF bytes return EAGAIN immediately.
	
	PipePair_st pp;
	pp.SetBlockingMode(Rblocking, Wnonb);

	int pipesize = 0, atomic_size = 0;
	QueryPrintPipesize(pp.fdw, pipesize, atomic_size);
	
	const int atomic2x = atomic_size * 2;
	const int wbuf_total = pipesize + atomic2x;
	// alloc a buffer that is wbuf_total. 
	Cec_delete_pchar cec_wbuf = new char[wbuf_total];
	const char *pwbuf = (char *)cec_wbuf;
//	const char *pwbuf_end_ = pwbuf + wbuf_total;
	
	int nwr = pipesize;
	printf("Write %d bytes to make pipe full.\n", nwr);
	int wret = write_ts(pp.fdw, pwbuf, nwr);
	assert(wret == nwr);

	// Write again will fail with EAGAIN.
	printf("Write again, this time more than PIPE_BUF bytes.\n");
	int ret_errno = 0, msec_use = 0;
	wret = write_ts(pp.fdw, pwbuf+nwr, atomic2x, &msec_use, ExLine, &ret_errno);
	assert(wret == -1 && ret_errno == EAGAIN);
	assert_shorter_than(msec_use, 0);
}

void NearFull_BigWrite_WNB(int nvacant, int bigwrite_bytes)
{
	// When pipe is near full(nvacant available space), 
	// Try writing >PIPE_BUF bytes, and see whether write()
	// return positive or EAGAIN.
	
	PipePair_st pp;
	pp.SetBlockingMode(Rblocking, Wnonb);

	int pipesize = 0, atomic_size = 0;
	QueryPrintPipesize(pp.fdw, pipesize, atomic_size);
	
	if (nvacant > pipesize)
		nvacant = pipesize;
	
	const int wbuf_total = pipesize + bigwrite_bytes;
	// alloc a buffer that is wbuf_total. 
	Cec_delete_pchar cec_wbuf = new char[wbuf_total];
	const char *pwbuf = (char *)cec_wbuf;
//	const char *pwbuf_end_ = pwbuf + wbuf_total;
	
	int nwr = pipesize-nvacant;
	printf("Write %d bytes to make pipe (near) full.\n", nwr);
	int wret = write_ts(pp.fdw, pwbuf, nwr);
	assert(wret == nwr);

	printf("Write again, this time more than PIPE_BUF bytes.\n");
	assert(bigwrite_bytes > atomic_size);

	int ret_errno = 0, msec_use = 0;
	wret = write_ts(pp.fdw, pwbuf+nwr, bigwrite_bytes, &msec_use, ExLine, &ret_errno);
//	assert(wret == -1 && ret_errno == EAGAIN);
	assert_shorter_than(msec_use, 0);
}

void NearFull_BigWrite_Find_BWTHRES()
{
	int fdpair[2];
	int err = pipe(fdpair);
	assert(!err);
	
	int pipesize, atomic_size;
	PipeSize_ret psret = GetPipeSize(fdpair[1], &pipesize, &atomic_size);
	assert(psret == PipeAtomic_HasLimit);
	
	int atomic2x = atomic_size * 2;
	
	char ibuf[80];
	printf("Input initial vacant bytes(0-%d): ", pipesize);
	Fgets(ibuf, sizeof(ibuf), stdin);
	
	int nvacant = atoi(ibuf);
	if (nvacant>=0 && nvacant<=pipesize)
	{
		printf("Input big-write bytes(%d): ", atomic2x);
		Fgets(ibuf, sizeof(ibuf), stdin);
		int bws = atoi(ibuf);
		if (bws <= 0)
			bws = atomic2x;
		else if (bws <= atomic_size)
		{
			printf("Input error. Big-write bytes should >%d\n", atomic_size);
			return;
		}

		NearFull_BigWrite_WNB(nvacant, bws);	
	}
	else 
	{
		printf("Input invalid.\n");
	}
	
	close(fdpair[0]);
	close(fdpair[1]);
}

enum NbBigWrite_et 
{
	Nbbw_custom = 0,
	Nbbw_atomic2x = 1,
	Nbbw_PipesizeEx = 2,
};

void NearFull_wNonAtomic_Hustle(NbBigWrite_et bwstyle, int bigwrite_bytes=0)
{
	// When pipe is near full(e.g. 100 bytes vacant, but vacant<atomic_size), 
	// write >PIPE_BUF bytes will ...
	
	PipePair_st pp;
	pp.SetBlockingMode(Rnonb, Wnonb);

	int pipesize = 0, atomic_size = 0;
	QueryPrintPipesize(pp.fdw, pipesize, atomic_size);

	const int extra = BIGWRITE_EXTRA;
	const int pipesize_ex = pipesize + extra;
	const int atomic2x = atomic_size * 2;
	
	if (bwstyle == Nbbw_atomic2x)
		bigwrite_bytes = atomic2x;
	else if (bwstyle == Nbbw_PipesizeEx)
		bigwrite_bytes = pipesize_ex;
	else // custom size
	{
		if (bigwrite_bytes < atomic_size)
		{
			printf("Error: Input big-write bytes must be at least atomic_size(%d).\n", atomic_size);
			return;
		}
		if (bigwrite_bytes > pipesize_ex)
		{
			printf("Error: Input big-write bytes must not exceed PipesizeEx(%d).\n", pipesize_ex);
			return;			
		}
	}
	
	const int wbuf_total = pipesize + pipesize_ex;
	// alloc a buffer that is wbuf_total. 
	Cec_delete_pchar cec_wbuf = new char[wbuf_total];
	const char *pwbuf = (char *)cec_wbuf;
//	const char *pwbuf_end_ = pwbuf + wbuf_total;
	
	const int vacant1 = 100;
	int nwr1 = pipesize - vacant1;
	printf("Write %d bytes to make pipe %d bytes(<PIPE_BUF) vacant.\n", nwr1, vacant1);
	int wret = write_ts(pp.fdw, pwbuf, nwr1);
	assert(wret == nwr1);

	const int rbuf_total = wbuf_total;
	const int flow_size = atomic_size - vacant1;
	Cec_delete_pchar cec_rbuf = new char[rbuf_total];
	char *prbuf = (char *)cec_rbuf;
	WorkIO_st ar_wkio [] = {
		{MSEC_INDEF_WAIT, WkioRead, flow_size},  // so that vacant size increase to =atomic_size
		{MSEC_INDEF_WAIT, WkioRead, nwr1-flow_size}, // so the pipe becomes empty(draining nwr1)
		{MSEC_SHORT_WAIT, WkioRead, rbuf_total-nwr1},
	};
	assert(nwr1-flow_size == pipesize-atomic_size);
	WorkIOs_st workios = {pp.fdr, NULL, prbuf, ar_wkio, GetEleQuan(ar_wkio)};
	CWorkIOThread iothread(workios);
	
	// <Step 1> Check whether write() can complete <PIPE_BUF.
	
	int nwr2 = bigwrite_bytes;
	assert(nwr2 >= atomic_size);
	printf("Write again, this time more than PIPE_BUF bytes(we use %d).\n", nwr2);
	int ret_errno = 0, msec_use = 0;
	wret = write_ts(pp.fdw, pwbuf+nwr1, nwr2, &msec_use, ExLine, &ret_errno); //z
	if (wret == vacant1)
	{
		// Not covered yet.
		printf("(Nonblocking big-write HUSTLE behavior detected. [Exact])\n");
		assert_shorter_than(msec_use, 0);
		return;
	}
	else if(wret > 0)
	{
		// Seen on openSUSE 13.1, with Nbbw_PipesizeEx .
		printf("Nonblocking big-write HUSTLE behavior detected. [NOT Exact]\n");
		assert_shorter_than(msec_use, 0);
		
		// Go on verify: read-bytes matches written.
		iothread.WaitThreadEnd();

		assert(ar_wkio[0].retbytes + ar_wkio[1].retbytes + ar_wkio[2].retbytes 
		       == nwr1 + wret);
		return;
	}
	
	assert(wret == -1);
	assert(ret_errno == EAGAIN);
	
	msleep(msec_bias_allow);
	
	// <Step 2> Check whether write() can complete =PIPE_BUF.
	
	msleep(MSEC_INDEF_WAIT); // wait for ar_wkio[0] to complete

	wret = write_ts(pp.fdw, pwbuf+nwr1, nwr2, &msec_use, ExLine, &ret_errno);
	if (wret == atomic_size)
	{
		// Not covered yet.
		printf("(Nonblocking big-write slight-LAZY behavior detected)\n");
		assert_shorter_than(msec_use, 0);
		return;		
	}

	assert(wret == -1);
	assert(ret_errno == EAGAIN);
	
	// <Step 3> Check whether write() can complete =PIPE_SIZE (F_GETPIPE_SZ)
	
	msleep(MSEC_INDEF_WAIT); // wait for ar_wkio[1] to complete
	
	wret = write_ts(pp.fdw, pwbuf+nwr1, pipesize_ex, &msec_use, ExLine, &ret_errno);
	if (wret > atomic_size)
	{
		// This is openSUSE linux 13.1 write() behavior on Nbbw_atomic2x. 
		// It refueses to complete even if vacant reaches atomic_size.
		printf("(Nonblocking big-write heavy-LAZY behavior detected.)\n");

		// Go on verify: read-bytes matches written.
		iothread.WaitThreadEnd();

		assert(ar_wkio[0].retbytes + ar_wkio[1].retbytes + ar_wkio[2].retbytes 
		       == nwr1 + wret);
		return;		
	}	
	else
	{
		printf("!!! Irrational return bytes from write(), return=%d.\n", wret);
		assert(0);			
	}
}

void NearFull_wNonAtomic_Hustle1()
{
	NearFull_wNonAtomic_Hustle(Nbbw_atomic2x);
}

void NearFull_wNonAtomic_Hustle2()
{
	NearFull_wNonAtomic_Hustle(Nbbw_PipesizeEx);
}

void NearFull_wNonAtomic_Hustle_Manual()
{
	int fdpair[2];
	int err = pipe(fdpair);
	assert(!err);
	
	int pipesize, atomic_size;
	PipeSize_ret psret = GetPipeSize(fdpair[1], &pipesize, &atomic_size);
	assert(psret == PipeAtomic_HasLimit);
	
	char ibuf[80];
	printf("Input big-write bytes(%d-%d): ", atomic_size, pipesize+BIGWRITE_EXTRA);
	Fgets(ibuf, sizeof(ibuf), stdin);
	
	int bigwrite = atoi(ibuf);
	NearFull_wNonAtomic_Hustle(Nbbw_custom, bigwrite);
	
	close(fdpair[0]);
	close(fdpair[1]);
}

void rClose_wSIGPIPE()
{
	PipePair_st pp;
	pp.SetBlockingMode(Rblocking, Wblocking);
	
	char wbuf[10], rbuf[10];
	const int nwr = 2, nrd = 10;
	
	int wret = write_ts(pp.fdw, wbuf, nwr);
	assert(wret == nwr);
	
	int ret = pp.CloseRead();
	assert(ret == 0);
	
	wret = write_ts(pp.fdw, wbuf, nwr); // The process should have been killed by SIGPIPE.
	
	printf("!!! Should not get here !!!\n");
	assert(0);
}

void rClose_wEPIPE(sighandler_t handler)
{
	if (handler == Sigpipe_handler)
		printf("Set a custom SIGPIPE handler.\n");
	else if (handler == SIG_IGN)
		printf("Set SIGPIPE handler to SIG_IGN.\n");
	
	Set_SignalHandler(SIGPIPE, handler);
	
	PipePair_st pp;
	pp.SetBlockingMode(Rblocking, Wblocking);
	
	char wbuf[10], rbuf[10];
	const int nwr = 2, nrd = 10;
	
	int wret = write_ts(pp.fdw, wbuf, nwr);
	assert(wret == nwr);
	
	int ret = pp.CloseRead();
	assert(ret == 0);
	
	int sigpipe_count = g_sigpipe_count;
	
	int ret_errno = 0;
	wret = write_ts(pp.fdw, wbuf, nwr, NULL, ExLine, &ret_errno);
	assert(wret == -1);
	assert(ret_errno == EPIPE); // yes, Linux does not return EINTR in this case
	
	if(handler==Sigpipe_handler)
		assert(sigpipe_count+1 == g_sigpipe_count);
	
	Set_SignalHandler(SIGPIPE, SIG_DFL);
}

void rClose_wEPIPE_handle()
{
	rClose_wEPIPE(Sigpipe_handler);
}
void rClose_wEPIPE_ignore()
{
	rClose_wEPIPE(SIG_IGN);
}


void BufNearFull_CheckWritable(WriteBlocking_et wside)
{
	// "NearFull" means vacant bytes are less than PIPE_BUF.
	// At this moment, we probe for writable. 
	// Do you think it notify writable or not?
	
	PipePair_st pp;
	pp.SetBlockingMode(Rblocking, wside);
	
	int pipesize = 0, atomic_size = 0;
	PipeSize_ret psret = GetPipeSize(pp.fdw, &pipesize, &atomic_size);
	assert(psret == PipeAtomic_HasLimit);
	PrintTs("Pipe-size %d, atomic-write-size %d (diff=%d).", 
		pipesize, atomic_size, pipesize-atomic_size);
	
	// alloc a buffer that is equal to pipesize+atomic_size.
	Cec_delete_pchar cecbuf = new char[pipesize+atomic_size];
	const char *pbuf = (char *)cecbuf;
	const char *pbuf_end_ = pbuf + pipesize+atomic_size;
	
	const int vacant1 = 100, vacant2 = 90;
	assert(pipesize > vacant1);
	
	int nwr = pipesize - atomic_size;
	printf("Now write %d to pipe to make it only %d(=PIPE_BUF) bytes vacant.\n", nwr, atomic_size);
	int wret = write_ts(pp.fdw, pbuf, nwr, NULL, NoExLine);
	assert(wret == nwr);
	//
	bool iswritable = IsFdWritable(pp.fdw);
	PrintTs("### Pipe writable: %s\n", iswritable ? "yes" : "no"); 
	assert(iswritable);
	
	printf("Now write to pipe to make it only %d bytes vacant.\n", vacant1);
	nwr = atomic_size - vacant1;
	wret = write_ts(pp.fdw, pbuf_end_-atomic_size, nwr, NULL, NoExLine);
	assert(wret == nwr);
	//
	iswritable = IsFdWritable(pp.fdw);
	PrintTs("### Pipe writable: %s\n", iswritable ? "yes" : "no"); 
		// On openSUSE 42.1, we get "no" here, same as TLPI p1343 table 63-5 says.
	assert(!iswritable);
	
	printf("Although not-writable now, write a less than vacant portion should succeed.\n");
	nwr = vacant1 - vacant2;
	wret = write_ts(pp.fdw, pbuf_end_-vacant1, nwr);
	assert(wret == nwr); // 
	
	iswritable = IsFdWritable(pp.fdw);
	assert(! iswritable);
	if(iswritable)
	{
		printf("Since it reports writable, we try to write PIPE_BUF(%d) bytes.\n", pipesize);
		wret = write_ts(pp.fdw, pbuf_end_-vacant2, atomic_size);
	}
}

void NearFull_CheckWritable_WB(){ BufNearFull_CheckWritable(Wblocking); }
void NearFull_CheckWritable_WNB(){ BufNearFull_CheckWritable(Wnonb); }


struct DelayFifoOpen_st
{
	const char *fifopath;
	int delay_msec;
	WhichSide_et whichside;
	
	int fd_return;
};

void DelayOpen_fifo_side(void *_param)
{
	DelayFifoOpen_st &di = *(DelayFifoOpen_st*)_param;
	bool isread = di.whichside == ReadSide;
	const char *side = isread ? "read" : "write";
	
	PrintTs("Delay %dms before opening FIFO %s-side...\n", di.delay_msec, side);
	msleep(di.delay_msec);
	
	PrintTs("Now opening FIFO %s-side...", side);
	di.fd_return = open(di.fifopath, isread?O_RDONLY:O_WRONLY);
	
	PrintTs("%s-side open() returns: %d", side, di.fd_return);
}

static void CreateFifoIfNotExist(const char *fifopath)
{
	if (access(fifopath, F_OK) != 0)
	{
		int err = mkfifo(fifopath, S_IRUSR | S_IWUSR);
		if (err)
		{
			PrintTsErr("mkfifo(\"%s\") fail.", fifopath);
			assert(0);
			return;
		}
	}
}

void Fifo_BlockingOpen(WhichSide_et blockside)
{
	const char *fifopath = "piperw.fifo";
	CreateFifoIfNotExist(fifopath);
	printf("Using FIFO path: \"%s\"\n", fifopath);
	
	bool isread = blockside == ReadSide ? true : false;
	const char *this_side = isread ? "read" : "write";
	
	DelayFifoOpen_st di = {fifopath, 1000, isread?WriteSide:ReadSide };
	
	GGT_HSimpleThread th = ggt_simple_thread_create(DelayOpen_fifo_side, &di);
	assert(th);
	
	PrintTs("Now opening FIFO %s-side...", this_side);
	int fd_thisside = open(fifopath, isread?O_RDONLY:O_WRONLY);
	PrintTs("%s-side open() returns: %d", this_side, fd_thisside);
	
	bool succ = ggt_simple_thread_waitend(th);
	assert(succ);
	
	assert(fd_thisside!=-1 && di.fd_return!=-1);
	
	close_ts(fd_thisside);
	close_ts(di.fd_return);
}

void Fifo_ReadSideBlockingOpen()  { Fifo_BlockingOpen(ReadSide); }
void Fifo_WriteSideBlockingOpen() { Fifo_BlockingOpen(WriteSide); }

void Fifo_NbWriteSideOpen()
{
	const char *fifopath = "piperw.fifo";
	CreateFifoIfNotExist(fifopath);
	printf("Using FIFO path: \"%s\"\n", fifopath);
	
	PrintTs("Now opening FIFO write-side with O_NONBLOCK ...");
	int fd = open(fifopath, O_WRONLY|O_NONBLOCK);
	int ret_error = errno;
	PrintTs("open() returns: %d, errno=%d(%s)", fd, ret_error, errno2name(ret_error));
	
	assert(ret_error == ENXIO);
	
	if (ret_error == ENXIO)
		printf("OK. ENXIO is expected.\n");
}

/////////////////////////////////////////////////////////////////////////

typedef void(*PROC_Case)(void);

enum OnlyManual_bt
{
	OnlyManual_false = 0,
	OnlyManual_true = 1,
};

struct Case_st
{
	PROC_Case fn;
	
	const char *brief;
	const char *detail;
	
	OnlyManual_bt only_manual;
};

#define CASE_STR(s) s, #s

Case_st g_cases[] =
{ 
	{ CASE_STR(wLess_rMore_RB), "Write less, Read more, blocking read."},
	{ CASE_STR(wLess_rMore_RNB), "Write less, Read more, nonblocking read." },
	{ CASE_STR(wWrite_wClose_r3times_RB), "Write, close write, then blocking read 3 times." },
	{ CASE_STR(wWrite_wClose_r3times_RNB), "Write, close write, then nonblocking read 3 times." },
	
	{ CASE_STR(wWait_Atomic_WB), "Write atomic size(WB), will block when vacant too small." },
	{ CASE_STR(wAtomic_SignalBreak_w0), "Write atomic size(WB), will return 0 byte on signal break." },
	{ CASE_STR(NearFull_wAtomic_EAGAIN_WNB), "Write atomic size(WNB), will return EGAIN." },
	
	{ CASE_STR(wNonAtomic_SignalBreak_FL), "Write >atomic_size, signal break returns none/partial." },
	{ CASE_STR(wNonAtomic_SignalBreak_FM), "Write >atomic_size, signal break returns partial/boundary." },
	{ CASE_STR(PipeFull_wNonAtomic_EAGAIN), "Write >atomic_size when pipe is full." },
	{ CASE_STR(NearFull_BigWrite_Find_BWTHRES), "Write >atomic_size(WNB), find BWTHRES.", OnlyManual_true },
	{ CASE_STR(NearFull_wNonAtomic_Hustle1), "Write >atomic_size when pipe near full 1 (BWTHRES)." },
	{ CASE_STR(NearFull_wNonAtomic_Hustle2), "Write >atomic_size when pipe near full 2 (BWTHRES)." },
	
	{ CASE_STR(NearFull_wNonAtomic_Hustle_Manual), "Write >atomic_size when pipe near full (manual).", OnlyManual_true },
	
	{ CASE_STR(rClose_wSIGPIPE), "Write to a read-end-closed pipe, killed by SIGPIPE.", OnlyManual_true }, 
	{ CASE_STR(rClose_wEPIPE_handle), "Write to a read-end-closed pipe, with handler, return EPIPE." }, 
	{ CASE_STR(rClose_wEPIPE_ignore), "Write to a read-end-closed pipe, with SIG_IGN, return EPIPE." }, 
	
	{ CASE_STR(NearFull_CheckWritable_WB), "Pipe buf near-full, is it writable? (WB)" },
	{ CASE_STR(NearFull_CheckWritable_WNB), "Pipe buf near-full, is it writable? (WNB)" },
	
	{ CASE_STR(Fifo_ReadSideBlockingOpen), "FIFO read-side blocking open." },
	{ CASE_STR(Fifo_WriteSideBlockingOpen), "FIFO write-side blocking open." },
	{ CASE_STR(Fifo_NbWriteSideOpen), "FIFO write-side nonblocking open returns ENXIO." },
};

bool do_manual_test(int use_case)
{
	int choice = use_case - 1; // choice is 0-based
	int ncases = GetEleQuan(g_cases);

	if(choice<0 || choice>=ncases)
	{
		// Let user manually pick a case.
		int i;
		for (i = 0; i < ncases; i++)
		{
			printf("%s[%d] %s (%s)\n", g_cases[i].only_manual?"-":" ",
				   i+1, g_cases[i].detail, g_cases[i].brief);
		}
		
		printf("\nSelect a case to try: ");
		
		char linebuf[40];
		Fgets(linebuf, sizeof(linebuf), stdin);
		if (linebuf[0] == '\r' || linebuf[0] == '\n')
		{
			printf("You select none. Quit.\n");
			return false;
		}
		
		choice = atoi(linebuf) - 1;
		if (choice < 0 || choice >= ncases)
		{
			printf("Invalid choice. Quit.\n");
			return false;
		}
	}
	
	const char *w10s = getenv("PIPERW_10S");
	if (w10s)
	{
		PrintTs("Waiting to start at 10-second boundary.");
		PrintTss(10*1000, "Start at 10-second boundary.\n");
	}
	
	printf("Case <#%d> %s\n", choice+1, g_cases[choice].brief);
//	printf("\n");
	
	reset_rw_seq();
	g_cases[choice].fn(); 
	
	return true;
}

void do_auto_test()
{
	int ncases = GetEleQuan(g_cases);
	int i, verify_count = 0;
	for (i = 0; i < ncases; i++)
	{
		mm_printf("%78n\n", "=");
		printf("Case <#%d> %s: %s\n", i+1, g_cases[i].brief, g_cases[i].detail);
		mm_printf("%78n\n", "=");

		if (g_cases[i].only_manual)
		{			
			printf("This is a manual case, skipped.\n\n");
			continue;
		}	
		
		reset_rw_seq();
		g_cases[i].fn(); 
		printf("\n");
		
		verify_count++;
	}
	
	printf("All %d cases verified OK (%d of which skipped).\n", ncases, ncases-verify_count);
}


int main(int argc, char *argv[])
{
	bool autotest = false;
	int use_case = 0; // 0 means to manual choose
	
	if(argc == 1)
	{
		printf("This program tests various pipe() read/write behavior.\n");
		printf("  -a: Auto-test all cases.\n");
		printf("  -4: Run case #4 immediately.\n");
	}
	else
	{	
		if (strcmp(argv[1], "-a") == 0)
			autotest = true;
		else if(argv[1][0]=='-')
		{
			use_case = atoi(argv[1]+1);
		}
	}
	
	int fdpair[2];
	int &fdr =  fdpair[0];
	int &fdw =  fdpair[1];
	
	int err = pipe(fdpair);
	if (err)
	{
		PrintTsErr("Calling pipe() fail.");
		exit(1);
	}

	PrintPipeBuf(fdw, "write");
	PrintPipeBuf(fdr, "read");
	close(fdr);
	close(fdw);
	
	printf("\n");
	
	Set_SignalHandler(SIGUSR1, null_handler);
	
	g_main_threadid = pthread_self();
	
	if (autotest)
	{
		do_auto_test();  // will assert if error inside
	}
	else
	{		
		bool b = do_manual_test(use_case);
	
		return b ? 0 : 1;
	}
}

