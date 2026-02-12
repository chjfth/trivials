#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <assert.h>
#include <xutility>

#include <EnsureClnup.h>


class CFoo
{
public:
	CFoo(int data) : m_data(data) {
		printf("CA ctor (%p)\n", this);
	}
	~CFoo() {
		printf("CA dtor (%p)\n", this);
	}
	int m_data;
};

MakeDelega_CleanupCxxPtr_en(CFoo, Cec_Foo, CecArray_Foo)

Cec_Foo grabobj(int i)
{
	CFoo *pobj = new CFoo(i);
	Cec_Foo cec = pobj;
	return cec;
}

void test2()
{
	Cec_Foo cecFoo = grabobj(33);
	assert(cecFoo->m_data == 33);
}

void test3()
{
	Cec_Foo cec2 = new CFoo(2);
	Cec_Foo cec3 = new CFoo(3);

	cec3 = std::move(cec3);
	cec2 = std::move(cec3);
}

struct ST4
{
	int kk;
	Cec_Foo cecfoo;
};

ST4 grab_st4()
{
	ST4 st4 = { 4 , grabobj(44) };
	
	return st4; // VC2010 bug: would use st4's copy-ctor!
}

void test4()
{
	ST4 st4obj = grab_st4();
	printf("in test4()\n");
}

int _tmain(int argc, TCHAR* argv[])
{
	printf("\n==test2()\n");
	test2();

	printf("\n==test3()\n");
	test3();

	printf("\n==test4()\n");
	test4();

	return 0;
}

