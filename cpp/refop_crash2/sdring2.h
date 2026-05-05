#ifndef __sdring2_h_
#define __sdring2_h_

class sdring
{
public:
	static int bufSize_(int nchars) { return nchars + 1; }

	sdring(int nchars)
	{
		_ct0r();
		if (nchars > 0)
			setbufsize(nchars);
	}

	sdring(const char* instr) {
		_ctor(instr);
	}

	sdring(const char* inchars, int nchars) {
		_ctor(inchars, nchars);
	}

public:
	// boilerplate code, no need to modify >>>
	sdring() { _ct0r(); }
	~sdring()
	{
		_dtor();
		_ct0r();
	}
	sdring(const sdring& old)            // copy-ctor
	{
		_copy_from_old(old);
	}
	sdring& operator=(const sdring& old) // copy-assign
	{
		if (this != &old) {
			_dtor();
			_copy_from_old(old);
		}
		return *this;
	}
	sdring(sdring&& old)            // move-ctor
	{
		_steal_from_old(old);
		old._ct0r();
	}
	sdring& operator=(sdring&& old) // move-assign
	{
		if (this != &old) {
			_dtor();
			_steal_from_old(old);
			old._ct0r();
		}
		return *this;
	}
	// boilerplate code, no need to modify <<<

private:
	void _copy_from_old(const sdring& old)
	{
		_ctor(old.m_buf);
	}

	void _steal_from_old(sdring& old) {
		m_buf = old.m_buf;
		m_nchars = old.m_nchars;
	}

public:
	sdring& operator=(const char* instr)
	{
		_dtor();
		_ctor(instr);
		return *this;
	}

	const char* c_str() const { // this has dual const decoration
		return m_buf;
	}

	char* getptr() { // this does not have const decoration
		return m_buf;
	}
	//  operator char* () {
	//		// I don't use this in favor of `operator char* & ()`.
	//		// Enable both will cause ambiguity in many user cases.
	//		return getptr();
	//  }

	char*& getbuf() {
		// Note: returns a reference(not char*'s value)
		// For function prototype, put a `const` exactly before `&` is vital.
		return m_buf;
	}
	operator char*& () {
		return getbuf();
	}

	operator char const* const& () {
		return getbuf();
	}

	const char* takeover(char* buf, int nchars)
	{
		// [2026-04-06] Used by makeTstring.h
		// Let sdring manage user-provided buf[], buf should be C++-delete-able.

		if (m_buf)
			delete[] m_buf;

		m_buf = buf;
		if (nchars >= 0)
			m_nchars = nchars;
		else
			m_nchars = str_len(m_buf);

		return m_buf;
	}

	char* setbufsize(int nchars) // to-test
	{
		if (nchars <= 0)
			nchars = 0;

		char* newbuf = new char[bufSize_(nchars)];
		newbuf[0] = newbuf[nchars] = '\0';

		if (m_buf)
		{
			// copy as-many-length of old string to newbuf
			int nchars_to_copy = m_nchars <= nchars ? m_nchars : nchars;
			for (int i = 0; i < nchars_to_copy; i++)
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

	int rawlen() const {
		return m_nchars;
	}

	char& operator[](int pos) {
		if (pos < 0)
			pos = m_nchars + pos; // count from backward
		return m_buf[pos];
	}

	const char& operator[](int pos) const {
		if (pos < 0)
			pos = m_nchars + pos; // count from backward
		return m_buf[pos];
	}

private:
	void _ctor(const char* instr)
	{
		// [2025-12-25] Note: We distinguish between 
		//	instr==NULL 
		// and
		//	instr[0]=='\0'

		_ct0r();

		if (instr)
		{
			m_nchars = str_len(instr); // m_nchars can be 0

			m_buf = new char[bufSize_(m_nchars)];
			str_cpy(m_buf, instr);
		}
	}

	void _ctor(const char* inchars, int nchars)
	{	// inchars[] may contain NUL, we always copy `nchars` count.
		_ct0r();
		if (inchars)
		{
			m_nchars = nchars;

			m_buf = new char[bufSize_(m_nchars)];
			m_buf[m_nchars] = '\0';

			for (int i = 0; i < nchars; i++)
				m_buf[i] = inchars[i];
		}
	}

public:
	static int str_len(const char s[])
	{
		int slen = 0;
		while (s[slen])
			slen++;
		return slen;
	}

	static int str_cpy(char dst[], const char src[])
	{
		int i = 0;
		for (; src[i]; i++)
			dst[i] = src[i];
		dst[i] = '\0'; // append NUL char
		return i; // i is string len
	}

	//
	// Place private members at end.
	//
private:
	int m_nchars;
	char* m_buf; // buffer size is m_nchars+1

private:
	void _ct0r()
	{
		m_buf = nullptr;
		m_nchars = 0;
	}
	void _dtor()
	{
		delete[] m_buf;
	}
};


#endif