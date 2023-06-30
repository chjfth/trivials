#include <stdio.h>
#include <stdlib.h>

class C1
{
public:
	C1()  { printf("C1-ctor()\n"); }
	~C1() { printf("C1-dtor()\n"); }
};

void do_bad()
{
	throw 3;
}

void cxx_raii()
{
	C1 c1obj;
	do_bad();
}

int main(int argc, char* argv[])
{
	printf("RaiiCatch0...\n");
	cxx_raii();
	return 0;
}
