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

DEFINE_CONTEXT_SETTER(SAcontext_set, SA, SAcontext)
DEFINE_CONTEXT_GETTER(SAcontext_get, SA, SAcontext)

DEFINE_CONTEXT_SETTER(SBcontext1_set, SB, SBcontext1)
DEFINE_CONTEXT_GETTER(SBcontext1_get, SB, SBcontext1)

DEFINE_CONTEXT_SETTER(SBcontext2_set, SB, SBcontext2)
DEFINE_CONTEXT_GETTER(SBcontext2_get, SB, SBcontext2)

int main(int argc, char* argv[])
{
	SA aobj = {1};
	SAcontext actx = {100};
	
	SAcontext_set(&aobj, &actx);

	SAcontext *pactx = SAcontext_get(&aobj);
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

