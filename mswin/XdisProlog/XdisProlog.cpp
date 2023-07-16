#include <stdio.h>

int func2()
{
	int retv = 0;
	retv += 3;
	return retv;
}

int func1()
{
	int retv = func2();
	return retv;
}

int main(int argc, char* argv[])
{
	int retv = func1();
	return retv;
}
