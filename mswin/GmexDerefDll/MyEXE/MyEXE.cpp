#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <assert.h>
#include <windows.h>

#include <MyDLL1.h>
#include <MyDLL2.h>

void test_GetModuleHandleEx(void *funcaddr, void *dll_addr)
{
	HMODULE hmod = NULL;
	BOOL b = GetModuleHandleEx(
		GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)funcaddr, &hmod);
	assert(b);
	assert(hmod==dll_addr);
	b = FreeLibrary(hmod); // Proved: We need this FreeLibrary.
	assert(b);
}

bool call_mydll2()
{
	HMODULE hDll = LoadLibrary(_T("MyDLL2.dll"));
	assert(hDll);

	decltype(MyDLL2_printversion) *print2v = nullptr;
	print2v = (decltype(MyDLL2_printversion)*)GetProcAddress(hDll, "MyDLL2_printversion");
	print2v();

	test_GetModuleHandleEx(print2v, hDll);

	BOOL succ = FreeLibrary(hDll);
	if(!succ)
	{
		printf("call_mydll2() FreeLibrary fail. WinErr=%d\n", GetLastError());
	}
	return succ ? true : false;
}


int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	
	MyDLL1_version_st vst1 = {};
	MyDLL1_getversion(&vst1);

	printf("EXE: Using MyDLL1 version: %d.%d.%d.%d\n", vst1.major, vst1.minor, vst1.patch, vst1.extra);

	call_mydll2();

/*
	MyDLL2_version_st vst2 = {};
	MyDLL2_getversion(&vst2);

	printf("EXE: Using MyDLL2 version: %d.%d.%d.%d\n", vst2.major, vst2.minor, vst2.patch, vst2.extra);
*/
	
	return 0;
}

