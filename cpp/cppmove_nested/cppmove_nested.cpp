#include <stdio.h>
#include <utility>
#include "CA.h"

struct CB
{
#if 0
	CB(){}
	~CB(){}

	CB(CB&&) = default;
	CB& operator=(CB&&) = default;
#endif

	CA m_ca;
	int mb;
};

int main()
{
#ifdef _MSC_VER
	printf("Compile using _MSC_VER=%d\n", _MSC_VER);
#if _MSC_VER<1900
	printf("Note: You are using Visual C++ older than VC2015(pre-C++11 era), the code behavior is wrong.\n\n");
#endif
#endif
#ifdef __GNUC__
	printf("Compiling using GCC version %d.%d\n", __GNUC__, __GNUC_MINOR__);
#endif

	CB obj1, obj2;
	obj1 = std::move(obj2);
}

