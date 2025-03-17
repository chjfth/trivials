#include <stdio.h>

// Assisted by AI.

#define yield(state) do { \
		coroutine_state = state; \
		return;   \
	case state: ; \
 } while (0)

//static int ping_state = 0, pong_state = 0;  // Coroutine states (looks useless)

void ping() 
{
	static int coroutine_state = 0;
	switch (coroutine_state) {
	case 0:
		while (1) {
			printf("Ping.\n");
			yield(1);
		}
	}
}

void pong() 
{
	static int coroutine_state = 0;
	switch (coroutine_state) {
	case 0:
		while (1) {
			printf("Pong!\n");
			yield(1);
		}
	}
}

int main() 
{
	const int cycles = 3;
	for (int i = 0; i < cycles; i++) {  // Run 3 cycles of ping-pong
		ping();
		pong();
	}
	return 0;
}
