#include <stdio.h>
#include <stdlib.h>
#include <locale.h>

#include <MyLib.h>

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	
	MyLib_version_st vst = {};
	MyLib_getversion(&vst);

	printf("EXE: Using MyLib(lib) version: %d.%d.%d.%d\n", vst.major, vst.minor, vst.patch, vst.extra);

	return 0;
}

