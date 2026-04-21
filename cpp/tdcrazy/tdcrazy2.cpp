// tdcrazy2.cpp, Last updated: 2026-04-21
#undef NDEBUG
#include <stdio.h>
#include <assert.h>

class Szring
{
public:
	char* m_buf;
	int m_nchars;
public:
	Szring(const char *ins, int nlen)
	{
		m_nchars = nlen;
		m_buf = new char[nlen + 1];
		str_cpy(m_buf, ins);

		printf("[E]In Szring ctor(): &m_buf = <%p>\n", &m_buf);
		printf("[F]                   m_buf = <%p>\n", m_buf);
	}

	~Szring() {
		delete []m_buf;
	}

	operator char* & () // I just want it, anyway!
	{
		return m_buf;
	}

	operator char const* const& () // GCC faint
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


template<typename TString>
class Splitter
{
public:
	Splitter(const TString &s) : m_str(s)
	{
		printf("[G]In Splitter ctor(): m_str aiming value: <%p>\n", &m_str);
		printf("[H]                    m_str target value: <%p>\n",  m_str);
	}

public:
	const TString &m_str;
};


void test3()
{
	printf("==== tdcrazy2 ====\n");
	Szring sz("ABCDE", 5);

	Splitter<char const*> holdref(sz);

	assert( sz.m_buf ==  holdref.m_str); // [X] 
	assert(&sz.m_buf == &holdref.m_str); // [Y] 
}


int main(int argc, char *argv[])
{
	test3();
	return 0;
}
