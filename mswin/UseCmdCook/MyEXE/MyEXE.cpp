#include <stdio.h>
#include <stdlib.h>
//#include <tchar.h>
#include <locale.h>

#include <CmdCook.h>

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	
	CmdCook_version_st vst = {};
	CmdCook_getversion(&vst);

	printf("EXE: Using CmdCook version: %d.%d.%d.%d\n", vst.major, vst.minor, vst.patch, vst.extra);

	return 0;
}

