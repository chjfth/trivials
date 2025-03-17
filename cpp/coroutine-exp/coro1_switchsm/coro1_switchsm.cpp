#include <stdio.h>

#define yield(state) do { \
		coroutine_state = state; \
		return; \
	case state:; \
} while (0)

void coroutine() 
{
	static int coroutine_state = 0;

	switch (coroutine_state) {
	case 0:
		printf("Coroutine: Step 1\n");
	yield(1);
		printf("Coroutine: Step 2\n");
	yield(2);
		printf("Coroutine: Step 3\n");
	yield(3);
		printf("Coroutine: Done\n");
	}
	coroutine_state = 0;  // Reset for reuse
	return;
}

int main() 
{
	for (int i = 0; i < 4; i++) {
		coroutine();
	}
	return 0;
}

/* Output:

Coroutine: Step 1
Coroutine: Step 2
Coroutine: Step 3
Coroutine: Done

*/
