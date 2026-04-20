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
		m_buf = new char[nlen+1];
		str_cpy(m_buf, ins);

		printf("[A]In Szring ctor(): &m_buf = <%p>\n", &m_buf);
		printf("[B]                   m_buf = <%p>\n",  m_buf);
	}

	~Szring()
	{
		delete m_buf;
	}

	operator PCHAR& () {
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

#undef NDEBUG


void do_test()
{
	Szring sz("ABC", 3);

	SHoldRef holdref(sz);

	assert(sz.m_buf == holdref.m_szref);
	assert(&sz.m_buf != *(PCHAR**)&holdref);
}


int main(int argc, char *argv[])
{
	do_test();
	return 0;
}
