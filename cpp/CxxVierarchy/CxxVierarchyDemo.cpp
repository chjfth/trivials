#include <stdio.h>
#include <stdlib.h>

#include "CxxVierarchy.h"

/*
Limitations of this Vierarchy pattern:
(1) vwork()'s return value cannot be inspected by caller. So vwork() have to return
    info via an output parameter.
(2) vwork() can not be marked `virtual`, which will cause &CFoo1::vwork and &CFoo2::vwork 
    etc result in virtual dispatching.
	This greatly defeats the purpose of V-round calling of *Parent*'s same-name function.
*/


class CFoo1
{
public:
	void vwork(VSeq_t vseq, int param1, int param2)
	{
		printf("[vseq=%2d] CFoo1.vwork(%d, %d)\n", vseq, param1, param2);
	}
};

class CFoo2 : public CFoo1
{
public:
	void vwork(VSeq_t vseq, int param1, int param2)
	{
		auto vc = MakeVierarchyCall(this, &CFoo1::vwork, vseq, param1, param2);
		// -- Why I cannot write `&__super::vwork`?

		printf("[vseq=%2d] CFoo2.vwork(%d, %d)\n", vseq, param1, param2);
	}
};

class CFoo3 : public CFoo2
{
public:
	void vwork(VSeq_t vseq, int param1, int param2)
	{
		auto vc = MakeVierarchyCall(this, &CFoo2::vwork, vseq, param1, param2);
		// -- Why I cannot write `&__super::vwork`?

		printf("[vseq=%2d] CFoo3.vwork(%d, %d)\n", vseq, param1, param2);
	}
};


int main()
{
	printf("==== CFoo2 vwork ====\n");

	CFoo2 obj2;
	obj2.vwork(0, 20, 21);


	printf("==== CFoo3 vwork ====\n");

	CFoo3 obj3;
	obj3.vwork(0, 30, 31);

	return 0;
}

