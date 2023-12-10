#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	printf("argv[0] = %s\n", argv[0]);

	pid_t mypid = getpid();
	printf("getpid()  = %5d     (my process ID)\n", mypid);

	pid_t parentpid = getppid();
	printf("getppid() = %5d     (parent process ID)\n", parentpid);

	pid_t pgid = getpgrp();
	printf("getpgrp() = %5d     (process group ID)\n", pgid);

	pid_t orig_sid = getsid(0);
	printf("getsid(0) = %5d     (session ID)\n", orig_sid);

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
