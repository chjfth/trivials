#include <stdio.h>
#include <stdarg.h>

extern"C" int mm_printfA(const char *fmt, ...);
extern"C" int mm_printfW(const wchar_t *fmt, ...);
