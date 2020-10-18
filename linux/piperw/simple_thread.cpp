#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>


#include "simple_thread.h"

struct SwThreadParam
{
	PROC_ggt_simple_thread proc;
	void *param;
	pthread_t th;  // th: thread handle
	
	SwThreadParam(){ memset(this, 0, sizeof(this)); }
};


static void *
_LinuxThreadWrapper(void * param)
{
	SwThreadParam *pw = (SwThreadParam*)param;
	pw->proc(pw->param);
	return 0;
}


GGT_HSimpleThread 
ggt_simple_thread_create(PROC_ggt_simple_thread proc, void *param, int stack_size)
{
	SwThreadParam *pwp = new SwThreadParam;
	pwp->proc = proc;
	pwp->param = param;
	
	int err = 0;
	pthread_attr_t attr;
	
	if (stack_size > 0)
	{
		pthread_attr_init(&attr);
		
		size_t stksize = stack_size;
		pthread_attr_setstacksize(&attr, stksize);

		err = pthread_create(&pwp->th, &attr, _LinuxThreadWrapper, pwp);

		pthread_attr_destroy(&attr);
	}
	else
	{
		err = pthread_create(&pwp->th, NULL, _LinuxThreadWrapper, pwp);
	}

	if (!err)
		return (GGT_HSimpleThread)pwp;
	else
		return NULL;
}

bool 
ggt_simple_thread_waitend(GGT_HSimpleThread h)
{
	if (!h) {
		return false;
	}
	
	SwThreadParam *pwp = (SwThreadParam*)h;
	
	int err = pthread_join(pwp->th, NULL);
	if (!err)
	{
		delete pwp;
		return true;
	}
	else 
		return false;
}
