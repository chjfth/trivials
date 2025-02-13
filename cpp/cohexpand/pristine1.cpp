#include <stdio.h>

#define cox_MAKE_STRING(asymbol) #asymbol
#define coy_MAKE_STRING(s) cox_MAKE_STRING(s)

#define WRAP_B(s) [s]
#define WRAP_BQ(s) coy_MAKE_STRING( WRAP_B(s) )

const char *s_greet = WRAP_BQ(greet);
// -- We get a static string "[greet]".
// The beauti-fact is: we can #define greet to be any string from compiler command-line.
// For example:
//		cl  /D greet="Hello world!" pristine1.cpp
//		gcc -D greet='Hello world!' pristine1.cpp
//
// Output will be:
//		Greet: [Hello world!]

int main(int argc, char* argv[])
{
	printf("Greet: %s\n", s_greet);
	return 0;
}
