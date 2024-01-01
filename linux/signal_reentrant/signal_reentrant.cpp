#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>

void
errExit(const char* format, ...)
{
	va_list argList;

	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);

	exit(3);
}

#define __int64 long long

__int64
time64_millisec()
{
	timeval tv_abs;
	gettimeofday(&tv_abs, NULL);

	return (__int64)tv_abs.tv_sec * 1000 + tv_abs.tv_usec / 1000;
}

unsigned rela_millisec()
{
	static __int64 s_start_millisec = time64_millisec();

	__int64 rela = time64_millisec() - s_start_millisec;
	return (unsigned)rela;
}

static void
SIGINT_handler(int sig)
{
	static int s_count = 0;
	s_count++;

	int count = s_count; // save a local count

	// Now unmask SIGINT to make reentrant possible.
	//
	sigset_t unmask = {}, prevmask = {};
	sigemptyset(&unmask);
	sigaddset(&unmask, SIGINT);
	if (sigprocmask(SIG_UNBLOCK, &unmask, &prevmask) == -1)
		errExit("sigprocmask");

	unsigned ms = rela_millisec();
	printf("[#%d][%u.%03u] caught SIGINT.\n", count, ms/1000, ms%1000);

	sleep(1);

	ms = rela_millisec();
	printf("[#%d][%u.%03u] leave  SIGINT.\n", count, ms/1000, ms%1000);
}

int
main(int argc, char* argv[])
{
	struct sigaction sa = {};
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = SIGINT_handler;
	if (sigaction(SIGINT, &sa, NULL) == -1)
		errExit("sigaction");

	printf("SIGHUP handler setup done, which delays for 1 second for each call.\n");
	printf("You can press Ctrl+C multiple times to try it, then Ctrl+\\ to quit it.\n");
	

	for (;;) {
		pause(); // Wait for signals
		printf("Back to main()\n");
	}
}

