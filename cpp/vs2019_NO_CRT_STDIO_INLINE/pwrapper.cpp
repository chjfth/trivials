#include "pwrapper.h"

int mm_printfA(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int ret = vprintf(fmt, args);

	va_end(args);
	return ret;
}

int mm_printfW(const wchar_t *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	int ret = vwprintf(fmt, args);
	
	va_end(args);
	return ret;
}
