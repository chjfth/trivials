#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX_THREADS 32

typedef long long __int64;

__int64 ggt_time64_millisec()
{
	timeval tv_abs;
	gettimeofday(&tv_abs, NULL);
	return (__int64)tv_abs.tv_sec * 1000 + tv_abs.tv_usec / 1000;
}


void burn()
{
	volatile int n = 1000 * 1000;
	while (n > 0)
		n--;
}

struct threadparam_st
{
	int seconds;
};

void* hog1cpu(void* params)
{
	threadparam_st* tp = (threadparam_st*)params;
	int seconds = tp->seconds;

	__int64 msec_start = ggt_time64_millisec();
	__int64 msec_end = msec_start + seconds * 1000;

	for(;;)
	{
		__int64 msec_now = ggt_time64_millisec();
		if (msec_now >= msec_end)
			break;

		burn();
	}

	return nullptr;
}

void do_hog(int seconds, int threads)
{
	int i;
	threadparam_st tp = { seconds };
	
	pthread_t ar_threads[MAX_THREADS] = {};
	for(i=0; i<threads; i++)
	{
		int err = pthread_create(&ar_threads[i], nullptr, hog1cpu, &tp);
		if(err)
		{
			printf("pthread_create() error.\n");
			exit(4);
		}
	}

	for(i=0; i<threads; i++)
	{
		void* res = nullptr;
		pthread_join(ar_threads[i], &res);
	}
}

int main(int argc, char *argv[])
{
	if(argc==1)
	{
		printf("Usage:\n");
		printf("    hogcpu <seconds> [threads]\n");
		printf("Example:\n");
		printf("    hogcpu 3\n");
		printf("    hogcpu 3 2\n");
		exit(1);
	}

	int seconds = atoi(argv[1]);

	int threads = 1;
	if (argc > 2)
		threads = atoi(argv[2]);

	if (threads > MAX_THREADS)
		threads = MAX_THREADS;
	
	printf("Hogging CPU now, for %d seconds, using %d threads.\n", 
		seconds, threads);

	do_hog(seconds, threads);
	
	return 0;
}
