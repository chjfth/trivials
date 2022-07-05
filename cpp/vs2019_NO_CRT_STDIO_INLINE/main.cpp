#include "pwrapper.h"

// cl /MT /D _NO_CRT_STDIO_INLINE main.cpp pwrapper.cpp

void main()
{
	mm_printfA("What is %d?\n", 123);
	
	//locale_t t;
	_vsnprintf_l(NULL, 0,0, 0, NULL);
}

#if 0
void usedull()
{
	vprintf(NULL, NULL);
	vwprintf(NULL, NULL);
}
#endif

