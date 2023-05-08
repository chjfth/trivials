#include <stdio.h>
#include <stdlib.h>
//#include <tchar.h>
#include <locale.h>

#include <MyDll.h>

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	
	MyDll_version_st vst = {};
	MyDll_getversion(&vst);

	printf("EXE: Using MyDll version: %d.%d.%d.%d\n", vst.major, vst.minor, vst.patch, vst.extra);

	return 0;
}

