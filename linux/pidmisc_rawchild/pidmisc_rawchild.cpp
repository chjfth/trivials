/*
 * This program shows a *new* process's PID, PGID, SID and PPID.
 * By saying *new*, I mean it is newly fork()-ed.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

void print_pids()
{
	pid_t mypid = getpid();
	printf("getpid()  = %5d     (my process ID, PID)\n", mypid);

	pid_t pgid = getpgrp();
	printf("getpgrp() = %5d     (process group ID, PGID)\n", pgid);

	pid_t orig_sid = getsid(0);
	printf("getsid(0) = %5d     (session ID, SID)\n", orig_sid);

	pid_t parentpid = getppid();
	printf("getppid() = %5d     (parent process ID, PPID)\n", parentpid);
}

int main(int argc, char* argv[])
{
	pid_t pid = fork();
	if (pid < 0) {
		printf("[ERROR] fork() error.\n");
	}
	else if (pid == 0) {
		// child process

		sleep(1); // Let parent printf() first

		printf("[CHILD PROCESS]\n");
		print_pids();
	}
	else {
		// parent process

		printf("[INITIAL PROCESS]\n");
		print_pids();

		printf("\n");

		// Sleep 1.1 seconds, so that CHILD's output appears *before*
		// the parent exits and show Bash prompt.
		usleep(1100 * 1000); 
	}
	
	return 0;
}
