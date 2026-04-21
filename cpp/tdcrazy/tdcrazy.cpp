// tdcrazy.cpp, Last updated: 2026-04-21
#undef NDEBUG
#include <stdio.h>
#include <assert.h>

typedef char* PCHAR;

class Szring
{
public:
	PCHAR m_buf;
	int m_nchars;
public:
	Szring(const char *ins, int nlen)
	{
		m_nchars = nlen;
		m_buf = new char[nlen + 1];
		str_cpy(m_buf, ins);

		printf("[A]In Szring ctor(): &m_buf = <%p>\n", &m_buf);
		printf("[B]                   m_buf = <%p>\n", m_buf);
	}

	~Szring() {
		delete m_buf;
	}

	operator PCHAR& () // I just want it, anyway!
	{
		return m_buf;
	}

	static int str_cpy(char *dst, const char *src)
	{
		int i = 0;
		for (; src[i]; i++)
			dst[i] = src[i];
		dst[i] = '\0'; // append NUL char
		return i; // i is string len
	}

};

struct SHoldRef
{
	const PCHAR &m_szref;

	SHoldRef(const PCHAR& szref) : m_szref(szref)
	{
		printf("[C]In SHoldRef ctor(): PCHAR target's addr  is <%p>\n", *(PCHAR**)this);
		printf("[D]                    PCHAR target's value is <%p>\n", m_szref);
	}
};

void test1()
{
	printf("==== test1 ====\n");
	Szring sz("ABC", 3);

	SHoldRef holdref(sz);

	assert(sz.m_buf == holdref.m_szref);      // ok
	assert(&sz.m_buf == *(PCHAR**)&holdref);  // ok
}


//////////////////////////////////////////////////////////////////////////

template<typename TString>
class Splitter
{
public:
	Splitter(const TString &s) : m_str(s)
	{
		printf("[C]In Splitter ctor(): PCHAR target's addr  is <%p>\n", *(PCHAR**)this);
		printf("[D]                    PCHAR target's value is <%p>\n", m_str);
	}

public:
	const TString &m_str;
};


void test2()
{
	printf("==== test2 ====\n");
	Szring sz("ABC", 3);

	// Crazy: Replace `char*` with `PCHAR` will fix the problem. Why?
	Splitter<const char*> holdref(sz);

	assert(sz.m_buf == holdref.m_str);       // [X] ok
	assert(&sz.m_buf == *(PCHAR**)&holdref); // [Y] fail
}


int main(int argc, char *argv[])
{
	test1();

	printf("\n");

	test2();
	return 0;
}
