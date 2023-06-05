#include <stdio.h>
#include <stdexcept>

extern void will_throw();

void doWork1() noexcept
{
	printf("In doWork1()...\n");
	will_throw();
}

void doWork2()
{
	printf("In doWork2()...\n");
	will_throw();
}

int main(int argc, char *argv[])
{
	bool path1 = (argc > 1 && argv[1][0] == '1') ? true : false;
	
	printf("Start try{} ...\n");
	try
	{
		if(path1)
		{
			doWork1();
			doWork2();
		}
		else
		{
			doWork2();
			doWork1();
		}
	}
	catch (std::exception &e)
	{
		printf("[Caught!] e.what() is: %s\n", e.what());
	}
	
	printf("Safe return from will_throw().\n");
	
	return 0;
}
