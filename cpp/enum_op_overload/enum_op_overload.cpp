//from: http://www.informit.com/guides/content.aspx?g=cplusplus&seqNum=23
//with some modifications(reject use of iostream, etc).
#include <stdio.h>

enum Days
{
	Sunday,
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday,
	Saturday,
};

Days& operator++(Days& d, int) // postfix ++
{
	if (d == Saturday)
		return d = Sunday; //rollover
	
	int temp = d; //convert to an int
	return d = Days(++temp);
}

int main()
{
	Days day = Monday;
	for (;;) //display days as integers
	{
		printf("%d\n", day);
		day++; // overloaded ++
		
		if (day == Monday)
			break;
	}
	return 0;
}

/* Supported by:
	Visual C++ 6 SP6
	ARM SDT 2.50 with ARM C++ 1.10[Build number 80]
*/

