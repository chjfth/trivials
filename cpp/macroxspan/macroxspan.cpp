#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#ifndef sname
#define sname foo
#endif

#ifndef svalue
#define svalue bar
#endif

//////////////////////////////////

#define MAKE_NARROW_STRING_L2(s) #s
#define MAKE_NARROW_STRING(s) MAKE_NARROW_STRING_L2(s)

#define MAKE_WIDE_STRING_L2(s) L ## #s
#define MAKE_WIDE_STRING(s) MAKE_WIDE_STRING_L2(s)

void make_narrow_string()
{
	// Hope to get :
	//
	//	char *foo = "bar";
	//
	const char *sname = MAKE_NARROW_STRING(svalue);

	printf(MAKE_NARROW_STRING(sname) "=\"");
	printf(sname);
	printf("\"\n");
}

void make_wide_string()
{
	// Hope to get :
	//
	//	wchar_t *foo = L"bar";
	//
	const wchar_t *sname = MAKE_WIDE_STRING(svalue);

	wprintf(MAKE_WIDE_STRING(sname) L"=L\"");
	wprintf(sname);
	wprintf(L"\"\n");
}

int main()
{
	setlocale(LC_ALL, "");

	make_narrow_string();

	make_wide_string();

	return 0;
}
