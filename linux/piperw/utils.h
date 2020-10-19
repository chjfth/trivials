#ifndef __utils_h_
#define __utils_h_

#include <linux/errno2name.h>

#include "simple_thread.h"

typedef int Fhandle_t;

extern pthread_t g_main_threadid;

enum PrintExtraNewLine_et { NoExLine = 0, ExLine = 1 };

extern int g_write_seq, g_read_seq;

void reset_rw_seq();

int write_ts(int fd, const char *wbuf, int nwr, 
             int *p_msec_use=NULL, PrintExtraNewLine_et exline=ExLine, int *p_errno=NULL);

int read_ts(int fd, char *rbuf, int nrd, 
            int *p_msec_use=NULL, PrintExtraNewLine_et exline=ExLine, int *p_errno=NULL);
            
int close_ts(int fd, const char *hint=NULL, 
	PrintExtraNewLine_et exline=ExLine, int *p_errno=NULL);

__int64 SysMillisec64(void);


bool BlockSignal(int sig, bool isblock);

//bool prepare_SIGINT();

//void report_SIGINT_count();

	
unsigned int ggt_get_mtick(unsigned int sync_boundary=0);
	// 'm' implies millisecond. This function has the same idea as Windows GetTickCount.
	// if(sync_boundary==1000), this function will wait for 0~999 millisec 
	// so that the return value will be xxxx000, xxxx001, xxxx008 etc, for easier
	// human observation.

void PrintTss(int sync_boundary, const char *fmt, ...);
void PrintTs(const char *fmt, ...);
void PrintTs100(const char *fmt, ...);

void PrintTsErr(const char *fmt, ...);

bool set_nonblock_flag(int desc, bool enable);

inline void msleep(int ms){ usleep(ms * 1000); }

enum PipeSize_ret
{
	PipeAtomic_Error = -1,
	PipeAtomic_NoLimit = 0,
	PipeAtomic_HasLimit = 1,
};

void PrintPipeBuf(int fd, const char *hint);

PipeSize_ret GetPipeSize(int fd, int *p_pipe_size, int *p_atomic_size);

bool IsFdWritable(int fd);

//const int MY_SIGNAL = SIGUSR1;

enum WorkIO_et
{
	WkioRead = 0, 
	WkioWrite = 1,
	WkioClose = 2,
	WkioSignal = 3 // send a SIGUSR1
};

struct WorkIO_st
{
	// Input:
	int delay_msec;
	WorkIO_et op;
	int reqbytes;  // bytes to read()/write()

	// Output:
	int retbytes;  // bytes returned from read()/write()
	int ret_errno; // errno if error occurs
};

struct WorkIOs_st
{
	int fd; // file handle to do I/O
	const char *wbuf;
	char *rbuf;
	
	WorkIO_st *arWorkIO;
	int nWorkIO;
};

class CWorkIOThread 
{
public:
	CWorkIOThread(WorkIOs_st &w);
	~CWorkIOThread();
	
	void WaitThreadEnd();
	
private:
	WorkIOs_st &workios;
	
	GGT_HSimpleThread m_hthread;
	
	void thread_start();
};

const int msec_bias_allow = 20;

inline bool IsLongerThan(int delta, int expect)
{
	if (delta > expect - msec_bias_allow)
		return true;
	else
		return false;
}

inline bool IsShorterThan(int delta, int expect)
{
	if (delta < expect + msec_bias_allow)
		return true;
	else
		return false;
}

void Set_SignalHandler(int signum, sighandler_t proc);


#define assert_longer_than(delta, expect) \
	if(! IsLongerThan(delta, expect) ) { \
		printf("!!! This case uses too little time: %dms, expect: %dms\n", delta, expect); \
		assert( IsLongerThan(delta, expect) ); \
	}

#define assert_shorter_than(delta, expect) \
	if(! IsShorterThan(delta, expect) ) { \
		printf("!!! This case uses too much time: %dms, expect: %dms\n", delta, expect); \
		assert( IsShorterThan(delta, expect) ); \
	}

void Fgets(char *s, int size, FILE *stream);


#endif
