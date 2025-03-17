#include <stdio.h>
#include <setjmp.h>

// [2025-03-17] GPT4o provided wrong code. 

jmp_buf buf;
int state = 0;

void coroutine() {

	if (setjmp(buf) == 0) 
		return; // Initial call
	
	printf("Step 1\n");
	if (setjmp(buf) == 0) 
		return;
	
	printf("Step 2\n");
	if (setjmp(buf) == 0) 
		return;
	
	printf("Step 3\n");
}

int main() {
	while (state < 3) {
		coroutine();
		longjmp(buf, 1);
		state++;
	}
	return 0;
}
