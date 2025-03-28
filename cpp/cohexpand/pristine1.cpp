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

int main()
{
	printf("Greet: %s\n", s_greet);

	return 0;
}

// Below code is for preprocessing(/P) investigation. 
// You can temporarily change your code to execute them if you will.

#define coy_WRAP_BQ(s) cox_MAKE_STRING( WRAP_B(s) )

void do_compare()
{
	const char *cox = cox_MAKE_STRING( WRAP_B(greet) );
	printf("cox = %s\n", cox);
	// cox = WRAP_B(hello world)

	const char *coy = coy_MAKE_STRING( WRAP_B(greet) );
	printf("coy = %s\n", coy);
	// coy = [greet]

	const char *coz = coy_WRAP_BQ( greet ); // C99spec text vague!
	printf("coz = %s\n", coz);
	// coz = WRAP_B(hello world)
}

