#include <stdio.h>
#include <string>
#include <memory>

using namespace std;

int ptr_diff(void* p1, void* p2)
{
	return (int)((char*)p1 - (char*)p2);
}

void test1()
{
	string* pNico2 = new string("nico2");
	string* pJutta2 = new string("jutta2");

	shared_ptr<string> spNico1 = make_shared<string>("nico1");
	shared_ptr<string> spJutta1 = make_shared<string>("jutta1");

	shared_ptr<string> spNico2{ pNico2 };
	shared_ptr<string> spJutta2{ pJutta2 };

	printf("sizeof(&spNico1)=%d , sizeof(spNico1)=%d\n",
		(int)sizeof(&spNico1), (int)sizeof(spNico1));

	printf("\n");

	printf("&spNico1  = %p\n", &spNico1);
	printf("&spJutta1 = %p\n", &spJutta1);
	printf("&spNico2  = %p\n", &spNico2);
	printf("&spJutta2 = %p\n", &spJutta2);

	printf("\n");

	printf("  pNico2=%p  ,  pJutta2=%p\n", pNico2, pJutta2);

	printf("\n");

	typedef void* PVOID;
	PVOID* pp1 = (PVOID*)&spNico1;
	printf("spNico1  memdump: %p , %p (diff:%d)\n", 
		pp1[0], pp1[1], ptr_diff(pp1[0], pp1[1]) );

	PVOID* pp2 = (PVOID*)&spNico2;
	printf("spNico2  memdump: %p , %p (diff:%d)\n", 
		pp2[0], pp2[1], ptr_diff(pp2[0], pp2[1]));

	printf("\n");

	pp1 = (PVOID*)&spJutta1;
	printf("spJutta1 memdump: %p , %p (diff:%d)\n", 
		pp1[0], pp1[1], ptr_diff(pp1[0], pp1[1]));

	pp2 = (PVOID*)&spJutta2;
	printf("spJutta2 memdump: %p , %p (diff:%d)\n", 
		pp2[0], pp2[1], ptr_diff(pp2[0], pp2[1]));
}

int main(int argc, char *argv[])
{
	test1();
}
