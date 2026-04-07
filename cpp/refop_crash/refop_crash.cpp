#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sdring1.h"
#include "StringSplitter.h"

void test1()
{
	sdring alltext("abc\ndef");
	StringSplitter<char*> sp(alltext);
	// StringSplitter<sdring> sp(alltext); // this is OK
	
	int len = 0;
	int pos1 = sp.next(&len);
	int pos2 = sp.next(&len);

	assert(pos1==0 && pos2==4);
	printf("Word1 at %d, word2 at %d\n", pos1, pos2);
}

int main()
{
	printf("Be aware, this C++ program will NOT crash on Debug-build,\n");
	printf("          but will CRASH on Release-build (/O2 or -O2).\n");
	test1();
	printf("Good, Survived.\n");
}

// To see crash on gcc 9.3, compile it using:
//	g++ -O2  refop_crash.cpp
