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
	int ar[1] = {};
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

void my_delete_string(string *s)
{
	delete s;
}

void test3()
{
	string* pNico3 = new string("nico3");
	shared_ptr<string> spNico3{ pNico3, my_delete_string };
}


class CMyDeleteString
{
public:
	void operator()(string *s)
	{
		delete s;
	}
};

void test4()
{
	string* pNico4 = new string("nico4");
	shared_ptr<string> spNico4{ pNico4, CMyDeleteString() };
}


int main(int argc, char *argv[])
{
	test1();
	test3();
	test4();
}
