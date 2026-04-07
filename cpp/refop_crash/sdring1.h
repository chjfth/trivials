#ifndef __sdring1_h_
#define __sdring1_h_

template<typename T_CHAR>
class sdring
{
public:

	static int bufSize_(int nchars){ return nchars+1; }

	sdring(int nchars=0)
	{
		m_nchars = 0;
		m_buf = nullptr;

		if(nchars>0)
			setbufsize(nchars);
	}

	sdring(const T_CHAR* instr)	{
		_ctor(instr);
	}

	sdring(const sdring& ins)            // copy-ctor
	{
		_ctor(ins.get());
	}
	
	sdring& operator=(const T_CHAR *instr)
	{
		delete[] m_buf;
		_ctor(instr);
		return *this;
	}

	sdring& operator=(const sdring& ins) // copy-assign
	{
		delete[] m_buf;
		_ctor(ins);
		return *this;
	}

	void _steal_from_old(sdring& old)
	{
		this->m_buf = old.m_buf;
		this->m_nchars = old.m_nchars;

		old.m_buf = nullptr;
		old.m_nchars = 0;
	}

	sdring(sdring&& old)            // move-ctor
	{
		_steal_from_old(old);
	}

	sdring& operator=(sdring&& old) // move-assign
	{
		if (this != &old) {
			delete[] this->m_buf;
			_steal_from_old(old);
		}
		return *this;
	}

	~sdring() 
	{
		delete[] m_buf;
		m_buf = nullptr;
		m_nchars = 0;
	}

	const T_CHAR* getptr() const {
		return m_buf;
	}

	T_CHAR* const& getbuf() const {
		// Note: returns a reference(not T_CHAR*'s value)
		// For function prototype, put a `const` exactly before `&` is vital.

		return m_buf; 
	}

	operator T_CHAR* const& () const { 
		return getbuf();
	}

	const T_CHAR* takeover(T_CHAR *buf, int nchars)
	{
		// [2026-04-06] Used by makeTstring.h

		if(m_buf)
			delete[] m_buf;

		m_buf = buf;
		if(nchars>=0)
			m_nchars = nchars;
		else
			m_nchars = str_len(m_buf);

		return m_buf;
	}

	T_CHAR* setbufsize(int nchars) // to-test
	{
		if(nchars<=0)
			nchars = 0;

		T_CHAR *newbuf = new T_CHAR[bufSize_(nchars)];
		newbuf[0] = newbuf[nchars] = '\0';

		if(m_buf)
		{
			// copy old string to newbuf
			int nchars_to_copy = m_nchars<=nchars ? m_nchars : nchars;
			for(int i=0; i<nchars_to_copy; i++)
				newbuf[i] = m_buf[i];
			
			newbuf[nchars_to_copy] = '\0';

			delete[] m_buf;
			m_buf = newbuf;
			m_nchars = nchars_to_copy;
		}
		else
		{
			m_buf = newbuf;
			m_nchars = nchars;
		}

		return m_buf;
	}

	T_CHAR* c_str() {
		return m_buf;
	}

	int len() {
		if(!m_buf || !m_buf[0])
			return 0;
		else
			return str_len(m_buf);
	}

	T_CHAR& operator[](int pos) {
		return m_buf[pos];
	}

	const T_CHAR& operator[](int pos) const {
		return m_buf[pos];
	}

	operator bool() const {
		return (m_buf && m_buf[0]) ? true : false;
	}


private:
	void _ctor(const T_CHAR *instr)
	{
		// [2025-12-25] Note: We distinguish btw 
		//	instr==NULL 
		// and
		//	instr[0]=='\0'

		m_nchars = 0;
		m_buf = nullptr;

		if (instr)
		{
			m_nchars = str_len(instr); // m_nchars can be 0

			m_buf = new T_CHAR[bufSize_(m_nchars)];
			str_cpy(m_buf, instr);
		}
	}

public:
	static int str_len(const T_CHAR s[])
	{
		int slen = 0;
		while(s[slen])
			slen++;
		return slen;
	}

	static int str_cpy(T_CHAR dst[], const T_CHAR src[])
	{
		int i=0;
		for(; src[i]; i++)
			dst[i] = src[i];
		dst[i] = '\0'; // append NUL char
		return i; // i is string len
	}

	static int str_ncpy(T_CHAR dst[], const T_CHAR src[], int maxlen)
	{
		// dst[] buffer should be maxlen+1
		if(maxlen<0)
			maxlen = 0;
		int i=0;
		for(; src[i] && i<maxlen; i++)
			dst[i] = src[i];
		dst[i] = '\0';
		return i;
	}

private:
	int m_nchars;
	T_CHAR* m_buf; // buffer size is m_nchars+1
};


#if defined(UNICODE) || defined(_UNICODE)
typedef sdring<wchar_t> Sdring;
#else
typedef sdring<char> Sdring;
#endif


#endif
