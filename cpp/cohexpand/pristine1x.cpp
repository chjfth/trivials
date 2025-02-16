#include <stdio.h>

#define cox_MAKE_STRING(asymbol) #asymbol
//#define coy_MAKE_STRING(s) cox_MAKE_STRING(s)

#define WRAP_B(s) [s]
#define WRAP_BQ(s) cox_MAKE_STRING( WRAP_B(s) )

const char *s_greet = WRAP_BQ(greet);

int main()
{
	printf("Greet: %s\n", s_greet);
	return 0;
}
