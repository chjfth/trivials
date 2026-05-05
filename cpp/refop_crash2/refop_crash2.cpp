#include <stdio.h>
#include "sdring2.h"

int print_charstr(const char* psz)
{
	return printf("charstr: %s\n", psz);
}

int test2()
{
	sdring sdtest("ABC");

	// This crashes if compiled with gcc-9.4, but OK with gcc-11.4 .
	int ret = print_charstr(sdtest);
	
	return ret;
}

int main(int argc, char *argv[])
{
	test2();
	return argc-1;
}

/* gcc-9.4 spouts warning like this:

sdring2.h(90,10): warning : returning reference to temporary [-Wreturn-local-addr]
sdring2.h(90,10): warning :    90 |   return m_buf;
sdring2.h(90,10): warning :       |          ^~~~~

*/