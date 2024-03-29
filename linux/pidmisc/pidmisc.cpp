#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	printf("argv[0] = %s\n", argv[0]);

	pid_t mypid = getpid();
	printf("getpid()  = %5d     (my process ID, PID)\n", mypid);

	pid_t pgid = getpgrp();
	printf("getpgrp() = %5d     (process group ID, PGID)\n", pgid);

	pid_t orig_sid = getsid(0);
	printf("getsid(0) = %5d     (session ID, SID)\n", orig_sid);

	pid_t parentpid = getppid();
	printf("getppid() = %5d     (parent process ID, PPID)\n", parentpid);

	printf("\n");

	char catcmd[80] = "";
	snprintf(catcmd, sizeof(catcmd), "cat /proc/%u/sessionid", (unsigned)mypid);
	printf("Now executing `%s`\n", catcmd);
	system(catcmd);

	printf("\n");

	pid_t new_sid = setsid();
	if (new_sid == -1)
		printf("setsid() fails to create new session-ID, errno=%d\n", errno);
	else
		printf("setsid() returns new session-ID: %d\n", new_sid);

	if(argc>1)
	{
		printf("== press Enter to end this exe ==\n");
		getchar();
	}
	
	return 0;
}
