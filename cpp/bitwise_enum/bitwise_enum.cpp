#include <stdio.h>

template<typename enum_type>
inline enum_type operator | (enum_type a, enum_type b) 
{
	return enum_type((int)a | b); 
}

enum TimeFormat_et
{
	UseDate = 0x1,
	UseHour = 0x2,
	UseSecond = 0x8,
	
	UseAll = UseDate | UseHour | UseSecond
/*
	This code compiles OK on VC2010 & gcc-11.3.
	
	But VC2015+ compiles error on UseAll assignment:
	
		error C2131: expression did not evaluate to a constant                                
		note: failure was caused by call of undefined function or one not declared 'constexpr'
	
	Workaround for VC2015+: Replace `inline` with `constexpr`.
*/
};

int main()
{
	printf("UseAll = 0x%X\n", UseAll);
	return 0;
}
