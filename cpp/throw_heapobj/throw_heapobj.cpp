#include <stdio.h>

class CBall // the exception object to throw
{
public:
	CBall(const char *i_msg) : errmsg(i_msg)
	{
		printf("CBall [%p] created.\n");
	}

	~CBall()
	{
		printf("CBall [%p] deleted.\n");
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
	printf("\n");

	try
	{
		my_work1();
	}
	catch (const CBall &ball)
	{
		printf("[CaughtIt] ball at: [%p]\n", &ball);
		printf("           errmsg : %s\n", ball.errmsg);

		// Q: Has autoball deleted? or a new obj?
	}

	printf("main() final line.\n");
}
