#ifndef __StringSplitter_h_
#define __StringSplitter_h_

inline bool StringSplitter_IsCrlf(int charval)
{
	return (charval == '\r' || charval == '\n') ? true : false;
}

inline bool StringSplitter_TrimNone(int)
{
	return false;
}

template<
	typename TString, // can be char* or wchar_t*, or any type that supports operator[] 
	bool IsSplitterChar(int charval) = StringSplitter_IsCrlf,
	bool IsTrimChar(int charval) = StringSplitter_TrimNone
>
class StringSplitter
{
public:
	StringSplitter(const TString &s, int startpos=0, int scanlen=-1)
		: m_str(s), m_startpos(startpos)
	{
		m_nowpos = m_startpos;
		//vaDbgTs(_T("StringSplitter.ctor(), &s=<%p>"), &s);

		if(scanlen>=0)
		{
			m_endpos_ = m_nowpos + scanlen;
		}
		else 
		{	// m_endpos determined by NUL char
			m_endpos_ = m_nowpos;
			while( m_str[m_endpos_] )
				m_endpos_++;
		}
	}

	void reset()
	{
		m_nowpos = m_startpos;
	}

	int next(int *p_nowlen=nullptr)
	{
		// Returns an offset value that points to the next sub-string.
		// *p_nowlen tells the length of the sub-string.

		// Skip consecutive splitter-chars and trim-chars.
		//
		for(;;)
		{
			if(m_nowpos == m_endpos_)
			{
				if(p_nowlen)
					*p_nowlen = 0;

				return -1; // scan ended
			}

			if( IsSplitterChar(m_str[m_nowpos]) || IsTrimChar(m_str[m_nowpos]))
				m_nowpos++;
			else
				break;
		}

		// Check if scan end.
		//
		if (m_nowpos == m_endpos_)
		{
			if (*p_nowlen)
				*p_nowlen = 0;

			return -1;
		}

		// Seen a word start, Scan for word length.

		int word_at_pos = m_nowpos;

		for(;;)
		{
			if( m_nowpos==m_endpos_ || IsSplitterChar(m_str[m_nowpos]) )
			{
				// Scan backward and drop those consecutive trim-chars.

				int tailpos = m_nowpos;
				while (IsTrimChar(m_str[tailpos - 1]))
					tailpos--;

				if(p_nowlen)
					*p_nowlen = tailpos - word_at_pos;

				return word_at_pos;
			}
			else
			{
				m_nowpos++;
			}
		}

	}

	int count()
	{
		reset();
		int n = 0;
		while (next() != -1)
			n++;

		reset();
		return n;
	}

// Disallow copy-assign, bcz C++ does not allow changing(rebinding) reference-target.
private:
	StringSplitter& operator=(const StringSplitter &insp) {}

private:
	const TString &m_str;

	int m_startpos;
	int m_nowpos;
	int m_endpos_;
};


#endif
