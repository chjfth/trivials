#include <stdio.h>

#define VIRTUAL virtual

typedef int VSeq_t;

class CFoo1
{
public:
	VIRTUAL void vwork(VSeq_t vseq, int param1, int param2)
	{
		printf("[vseq=%2d] CFoo1.vwork(%d, %d)\n", vseq, param1, param2);
	}

	void check();
};

class CFoo2 : public CFoo1
{
public:
	VIRTUAL void vwork(VSeq_t vseq, int param1, int param2)
	{
		printf("[vseq=%2d] CFoo2.vwork(%d, %d)\n", vseq, param1, param2);
	}

	void check();
};

class CFoo3 : public CFoo2
{
public:
	VIRTUAL void vwork(VSeq_t vseq, int param1, int param2)
	{
		printf("[vseq=%2d] CFoo3.vwork(%d, %d)\n", vseq, param1, param2);
	}

	void check();
};

void CFoo1::check()
{
	printf("==== CFoo1 check ====\n");
	printf("&CFoo1::vwork = %p \n", &CFoo1::vwork);
	printf("&CFoo2::vwork = %p \n", &CFoo2::vwork);
	printf("&CFoo3::vwork = %p \n", &CFoo3::vwork);
}

void CFoo2::check()
{
	printf("==== CFoo2 check ====\n");
	printf("&CFoo1::vwork = %p \n", &CFoo1::vwork);
	printf("&CFoo2::vwork = %p \n", &CFoo2::vwork);
	printf("&CFoo3::vwork = %p \n", &CFoo3::vwork);
}

void CFoo3::check()
{
	printf("==== CFoo3 check ====\n");
	printf("&CFoo1::vwork = %p \n", &CFoo1::vwork);
	printf("&CFoo2::vwork = %p \n", &CFoo2::vwork);
	printf("&CFoo3::vwork = %p \n", &CFoo3::vwork);
}

int main()
{
	CFoo1 obj1;
	obj1.check();

	CFoo2 obj2;
	obj2.check();

	CFoo3 obj3;
	obj3.check();
}


