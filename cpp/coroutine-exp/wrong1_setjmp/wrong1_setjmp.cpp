#include <stdio.h>
#include <setjmp.h>

// [2025-03-07] GPT4o provided wrong code. 

jmp_buf ctx1, ctx2;

void coroutine2() {
	printf("Coroutine 2: Step 1\n");
	longjmp(ctx1, 1);  // Switch back
	printf("Coroutine 2: Step 2\n");
	longjmp(ctx1, 1);  // Switch back
}

void coroutine1() {
	if (setjmp(ctx1) == 0) {
		coroutine2();
	}
	if (setjmp(ctx1) == 0) {
		printf("Coroutine 1: Step 1\n");
		longjmp(ctx2, 1);  // Switch to coroutine2
	}
	if (setjmp(ctx1) == 0) {
		printf("Coroutine 1: Step 2\n");
		longjmp(ctx2, 1);  // Switch again
	}
}

int main() {
	if (setjmp(ctx2) == 0) {
		coroutine1();
	}
	return 0;
}

/* Output:

Coroutine 2: Step 1
Coroutine 1: Step 1

*/
