/*
	Note: This code snippet is not compilable.
*/
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "api.h"

struct SAcontext {
	int usera;
};

struct SBcontext1 {
	int userb1;
};

struct SBcontext2 {
	int userb2;
};

int main(int argc, char* argv[])
{
	SA aobj = {1};
	SAcontext actx = {100};
	
	ContextSet(&aobj, &actx);

	SAcontext *pactx = ContextGet(&aobj); // compile error, my wrong idea
	assert(pactx==&actx);

	//// 

	SB bobj = {2};

	SBcontext1 bctx1 = {201};
	SBcontext1_set(&bobj, &bctx1);
	
	SBcontext1 *pbctx1 = SBcontext1_get(&bobj);
	assert(pbctx1==&bctx1);

	// SAcontext *pactx_wrong =  bobj.context; // compile error alert to user, OK

	SBcontext2 bctx2 = {202};
	SBcontext2 *pbctx2 = SBcontext2_get(&bobj); 
	// -- !!! BAD two types of context scrambled. 

	printf("Hello, struct_context stage1!\n");
	return 0;
}

