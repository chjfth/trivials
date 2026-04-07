#ifndef __sdring1_h_
#define __sdring1_h_

#include <string.h>

class sdring
{
public:
	static int bufSize_(int nchars){ return nchars+1; }

	sdring()
	{
		m_nchars = 0;
		m_buf = nullptr;
	}

	sdring(const char* instr)	{
		_ctor(instr);
	}

	sdring(const sdring& ins)            // copy-ctor
	{
		_ctor(ins.m_buf);
	}
	
	sdring& operator=(const char *instr)
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

#define USE_ERROR_CODE
#ifdef USE_ERROR_CODE
	operator char* () const {         // OPE1
		return m_buf; 
	}
// 	operator const char* &() const {  // OPE2
// 		return m_buf;
// 	}
#else
	operator char* const& () const {  // this is correct code
		return m_buf; 
	}
#endif

	char* c_str() { return m_buf; }

private:
	void _ctor(const char *instr)
	{
		m_nchars = 0;
		m_buf = nullptr;

		if (instr)
		{
			m_nchars = strlen(instr); // m_nchars can be 0

			m_buf = new char[bufSize_(m_nchars)];
			strcpy(m_buf, instr);
		}
	}

private:
	int m_nchars;
	char* m_buf; // buffer size is m_nchars+1
};


#endif
