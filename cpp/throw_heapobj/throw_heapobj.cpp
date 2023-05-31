#include <stdio.h>

class CBall // the exception object to throw
{
public:
	CBall(const char *i_msg) : errmsg(i_msg)
	{
		printf("CBall ctor, [%p] created.\n", this);
	}

	~CBall()
	{
		printf("CBall dtor, [%p] deleted.\n", this);
	}

	CBall(const CBall &oldball)
	{
		printf("CBall copy-ctor() [%p] -> [%p]\n", &oldball, this);
		errmsg = oldball.errmsg;
	}

	CBall(const CBall&& oldball)
	{
		printf("CBall move-ctor() [%p] -> [%p]\n", &oldball, this);
		errmsg = oldball.errmsg;
	}

	const char* errmsg;
};

class CAutoCleanup
{
public:
	CAutoCleanup(CBall* resptr, char *psneak)
		: m_resptr(resptr), m_psneak(psneak)
	{
		printf("CAutoCleanup register resource ptr [%p]\n", m_resptr);
	}
	~CAutoCleanup()
	{
		if(m_resptr)
		{
			printf("CAutoCleanup deleting resource ptr [%p]\n", m_resptr);
		}
		delete m_resptr;

		m_psneak[2] = 'z';
		m_psneak[3] = '\0';
	}
	
private:
	CBall* m_resptr;
	char* m_psneak;
};

void sth_wrong1()
{
	char localstr[] = "Bad happens in sth_wrong1()!";
	CBall *pball = new CBall(localstr);

	CAutoCleanup autoball(pball, localstr);
	
	throw(*pball);

	printf("This never execute after throw().\n");
}

void my_work1()
{
	printf("mywork1() first line.\n");
	
	sth_wrong1();

	printf("mywork1() final line.\n");
}

int main(int argc, char *argv[])
{
	const char* ptrexe = argv[0];
	printf("argv[0] = %s\n", ptrexe);
	printf("Localvar at: [%p]\n", &ptrexe);
	printf("\n");

	try
	{
		my_work1();
	}
	catch (const CBall &ball)
	{
		printf("[CaughtIt] excpt-obj at: [%p]\n", &ball);
		printf("           errmsg      : %s\n", ball.errmsg);

		// Q: Has autoball(from sth_wrong1) deleted? or a new obj?
	}

	printf("main() final line.\n");
}

/*
 * Key points of this toy-program:
 *
 * (1) In sth_wrong1(), it throws a heap object(an object created with `new`), instead of a temporary.
 *     the result is: C++ engine makes a copy of the thrown object(using copy-ctor) to create a
 *     "local object" at caught site.
 *     
 * (2) The lifetime of the original thrown object, obey the orginal rule when "no throw() is used".
 *
 * (3) At caught-site, Whether the excpt-obj contains [valid pointers to other members], is not cared
 *     by C++ engine. The coder should be responsible for it by himself.
 */
