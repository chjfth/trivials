#include <iostream>
#include <stdio.h>
#include <string>

// Try cases of throwing exception from ctor

using namespace std;

struct MyExcept : std::exception
{
	std::string emsg;
	MyExcept(const char *msg) : emsg(msg) {}
};

class ClassA
{
public:
	ClassA()
	{
		printf("ClassA-ctor()\n");
		
		m1 = 10;
		throw(MyExcept("Exception: m2 is not init-ed yet."));
	}

	~ClassA()
	{
		printf("ClassA-dtor()\n");
	}

private:
	int m1, m2;
};

void test_ClassA_nocatch()
{
	ClassA aobj;
}

void test_ClassA_catch()
{
	try {
		ClassA aobj;
	}
	catch(MyExcept &exc) {
		cout << "[caught] " << exc.emsg << endl;
	}
}

////////////////////////////////////////

class CChild1
{
public:
	CChild1()
	{
		printf("CChild1-ctor()\n");
	}
	~CChild1()
	{
		printf("CChild1-dtor()\n");
	}
};

class CChild2
{
public:
	CChild2()
	{
		printf("CChild2-ctor()\n");
		printf("CChild2 now throws in ctor...\n");
		throw(MyExcept("CChild2-ctor fails."));
	}
	~CChild2()
	{
		printf("CChild2-dtor()\n");
	}
};

class ClassB
{
public:
	ClassB()
	{
		printf("ClassB-ctor()\n");
	}
	~ClassB()
	{
		printf("ClassB-dtor()\n");
	}

private:
	CChild1 c1;
	CChild2 c2;
};

void test_ClassB_throw()
{
	try {
		ClassB bobj;
	}
	catch (MyExcept& exc) {
		cout << "[caught] " << exc.emsg << endl;
	}
}

int main(int argc, char *argv[])
{
	test_ClassA_nocatch();
	test_ClassA_catch();
//	test_ClassB_throw();
	
	printf("main() done.\n");
	return 0;
}
