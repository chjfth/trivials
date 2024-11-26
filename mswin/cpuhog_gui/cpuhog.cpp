#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include "cpuhog.h"

#define ggt_DELTA_EPOCH_IN_MILLISEC 11644473600000
// -- Number of milliseconds between the beginning of the Windows epoch (Jan. 1, 1601) 
// and the Unix epoch (Jan. 1, 1970) 

static __int64 FileTime_to_int64_millisec(const FILETIME *input)
{
	__int64 result;

	result = input->dwHighDateTime;
	result = result << 32;
	result |= input->dwLowDateTime;
	result /= 10000;    /* Convert from 100 nano-sec periods to milliseconds. */
	result -= ggt_DELTA_EPOCH_IN_MILLISEC;  /* Convert from Windows epoch to Unix epoch */

	return result;
}


__int64 ggt_time64_millisec()
{
	FILETIME time = {};
	GetSystemTimeAsFileTime(&time);
	return FileTime_to_int64_millisec(&time);
}


typedef struct _GGT_HSimpleThread_st { } *GGT_HSimpleThread;

typedef void (*PROC_ggt_simple_thread)(void *param);
// -- Note: On x86 Windows, this defaults to __cdecl, not __stdcall .

struct SwThreadParam
{
	PROC_ggt_simple_thread proc;
	void *param;
};

static unsigned __stdcall _WinThreadWrapper(void * param)
{
	SwThreadParam *pw = (SwThreadParam*)param;

	pw->proc(pw->param);
	delete pw;

	return 0;
}

GGT_HSimpleThread
ggt_simple_thread_create(PROC_ggt_simple_thread proc, void *param, int stack_size)
{
	HANDLE hThread = NULL;
	SwThreadParam *pwp = new SwThreadParam;
	if(!pwp)
		return NULL;

	pwp->proc = proc, pwp->param = param;

	unsigned tid = 0;
	hThread = (HANDLE)_beginthreadex(NULL, stack_size, _WinThreadWrapper, pwp ,0, &tid);

	return (GGT_HSimpleThread)hThread;
}

bool ggt_simple_thread_waitend(GGT_HSimpleThread h)
{
	if(!h) {
		return false;
	}

	DWORD waitre = WaitForSingleObject(h, INFINITE);
	if(waitre==WAIT_OBJECT_0)
	{
		CloseHandle(h);
		return true;
	}
	else
		return false;
}

template<typename TClass>
inline void cxx_threadbegin_stub(void *_param)
{
	struct SParam
	{
		TClass *obj;
		void (TClass::*Func)();
	} *param = (SParam*)_param;;

	TClass *obj = param->obj;
	void (TClass::*Func)() = param->Func;
	(obj->*Func)();

	delete param;
}

template<typename TClass>
inline GGT_HSimpleThread 
ggt_cxx_thread_create(TClass &cxxobj, void (TClass::*memberfunc)(), int stack_size=0)
{
	struct SParam
	{
		TClass *obj;
		void (TClass::*Func)();
	};

	SParam *param = new SParam; // will `delete` in cxx_threadbegin_stub()
	param->obj = &cxxobj;
	param->Func = memberfunc;

	GGT_HSimpleThread handle = 
		ggt_simple_thread_create(&cxx_threadbegin_stub<TClass>, param, stack_size);

	//	Sleep(2000);
	return handle;
}

//////////////////////////////////////////////////////////////////////////

static void burn()
{
	volatile int n = 1000 * 1000;
	while (n > 0)
		n--;
}


class CCpuHog
{
	int m_hog_millisec;
public:
	CCpuHog(int hog_millisec)
	{
		m_hog_millisec = hog_millisec;
	}

	void thread_start_hog()
	{
		__int64 msec_start = ggt_time64_millisec();
		__int64 msec_end = msec_start + m_hog_millisec;

		for(;;)
		{
			__int64 msec_now = ggt_time64_millisec();
			if (msec_now >= msec_end)
				break;

			burn();
		}		
	}
};

GGT_HSimpleThread gar_hthreads[MAX_THREADS];
CCpuHog *garp_hogs[MAX_THREADS];
int g_threads = 0;

bool start_cpu_hog(int threads, int seconds)
{
	if(garp_hogs[0])
	{
		// already started.
		return false;
	}

	if(threads<1)
		return false;

	g_threads = threads<=MAX_THREADS ? threads : MAX_THREADS;

	int i;
	for(i=0; i<threads; i++)
	{
		garp_hogs[i] = new CCpuHog(seconds*1000);
		if(!garp_hogs[i])
		{
			garp_hogs[0] = nullptr;
			return false;
		}

		gar_hthreads[i] = ggt_cxx_thread_create(*garp_hogs[i], &CCpuHog::thread_start_hog);
		if(!gar_hthreads[i])
		{
			return false;
		}
	}

	// Caller should later call cpu_hog_is_end() is free the resources.
	return true;
}

bool cpu_hog_is_end()
{
	int i;
	for(i=0; i<g_threads; i++)
	{
		if(gar_hthreads[i]==nullptr)
			break;

		DWORD waitre = WaitForSingleObject(gar_hthreads[i], 0); // just peek
		if(waitre==WAIT_OBJECT_0)
			continue;
		else if(waitre==WAIT_TIMEOUT)
			return false;
		else
			assert(0);
	}

	return true;
}

bool done_cpu_hog()
{
	int i;
	for(i=0; i<g_threads; i++)
	{
		ggt_simple_thread_waitend(gar_hthreads[i]);
		gar_hthreads[i] = nullptr;

		delete garp_hogs[i];
		garp_hogs[i] = nullptr;
	}

	return true;
}
