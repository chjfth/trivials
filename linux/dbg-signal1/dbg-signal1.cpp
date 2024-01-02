#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

void nested_print(const char* text)
{
	printf("%s\n", text);
}

void sig_handler(int sig)
{
	printf("In sig_handler(%d) %s ~\n", sig, strsignal(sig));
	nested_print("Are you OK?");
}


int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		printf("Press Enter to continue.\n");
		getchar(); // So that the debugger has time to attach it.
	}

	struct sigaction sa = {};
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = sig_handler;
	if (sigaction(SIGQUIT, &sa, NULL) == -1)
		exit(10);
	if (sigaction(SIGHUP, &sa, NULL) == -1)
		exit(20);
	if (sigaction(SIGINT, &sa, NULL) == -1)
		exit(30);

	printf("pause() waiting for signal...\n");
	pause();

	printf("pause() returned.\n");
	nested_print("You are Ooooooooook.");
}
