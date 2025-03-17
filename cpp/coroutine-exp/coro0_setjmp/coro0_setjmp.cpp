#include <stdio.h>
#include <setjmp.h>

/* Assisted by AI, with crucial Chj manual fix.

Prompt:

I'm a versed C programmer. But I meet a new concept today about "Donald Knuth's coroutine 
concept in C". Tell me something about it.

*/

jmp_buf ctx1 = {}; // Store resume-location in coroutine1()
jmp_buf ctx2 = {}; // Store resume-location in coroutine2()

void coroutine1() 
{
	printf("Coroutine 1: Step 1\n");
	int jmp1 = setjmp(ctx1);
	if(jmp1==0)
		longjmp(ctx2, 5);  // Switch back

	printf("Coroutine 1: Step 2\n");
	jmp1 = setjmp(ctx1);
	if(jmp1==0)
		longjmp(ctx2, 6);  // Switch back

	printf("Coroutine 1: Done.\n");

	// Chj: We must use longjmp() to *return*, instead of normal `return`,
	// otherwise, Windows will report access violation at address 0x22.
	longjmp(ctx2, 7); 
}

void coroutine2() 
{
	int jmp2 = setjmp(ctx2);
	if (jmp2==0) {
		coroutine1();
	}

	jmp2 = setjmp(ctx2);
	if (jmp2==0) {
		printf("Coroutine 2: Step 1\n");
		longjmp(ctx1, 0x11);  // Switch to coroutine1
	}

	jmp2 = setjmp(ctx2);
	if (jmp2==0) {
		printf("Coroutine 2: Step 2\n");
		longjmp(ctx1, 0x22);  // Switch again
	}

	printf("Coroutine 2: Done.\n");
}


int main() 
{
	coroutine2();
	return 0;
}

/* Output:

Coroutine 1: Step 1
Coroutine 2: Step 1
Coroutine 1: Step 2
Coroutine 2: Step 2
Coroutine 1: Done.
Coroutine 2: Done.

*/