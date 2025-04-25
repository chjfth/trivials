#pragma once

#if _MSC_VER < 1900
#error "This makedeeptree needs at least VC2015 to compile. Older compiler behaves abnormally on C++ exceptions."
#endif

#include <tchar.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

struct ErrMsg
{
	TCHAR *m_func;
	TCHAR *m_errmsg;

	ErrMsg(const TCHAR *func, const TCHAR *errmsg) :
		m_func(nullptr), m_errmsg(nullptr)
	{
		int slen = (int)_tcslen(func);
		m_func = new TCHAR[slen + 1];
		_tcscpy_s(m_func, slen+1, func);

		slen = (int)_tcslen(errmsg);
		m_errmsg = new TCHAR[slen + 1];
		_tcscpy_s(m_errmsg, slen+1, errmsg);
	}

	~ErrMsg()
	{
		delete m_func;
		delete m_errmsg;
	}
};

class char2TCHAR
{
public:
	char2TCHAR(const char *s)
	{
		if (sizeof(TCHAR) == 1)
		{
			m_achars = const_cast<char*>(s);
			m_isdelete = false;
		}
		else
		{
			int slen = strlen(s);
			m_wchars = new WCHAR[slen + 1];
			MultiByteToWideChar(0, 0, s, slen+1, m_wchars, slen+1);
			m_isdelete = true;
		}
	}
	
	~char2TCHAR()
	{
		if (m_isdelete)
			delete m_wchars;
	}

	operator const TCHAR*()
	{
		return m_tchars;
	}

private:
	union
	{
		char  *m_achars;
		WCHAR *m_wchars;
		TCHAR *m_tchars;
	};
	bool m_isdelete;
};

void throwErrMsg(const TCHAR *func, const TCHAR *fmt, ...)
{
	TCHAR tbuf[8000] = _T("");
	va_list args;
	va_start(args, fmt);
	_vsntprintf_s(tbuf, _TRUNCATE, fmt, args);
	va_end(args);

	throw ErrMsg(func, tbuf);
}

#define THROWE(fmt, ...) throwErrMsg(char2TCHAR(__func__), fmt, __VA_ARGS__)


inline bool IsBufferSmall(DWORD winerr)
{
	if( winerr==ERROR_INSUFFICIENT_BUFFER ||
		winerr==ERROR_MORE_DATA ||
		winerr==ERROR_BUFFER_OVERFLOW ||
		winerr==ERROR_BAD_LENGTH )
		return true;
	else
		return false;
}

inline bool IsSemicolon(int c)
{
	return c==';' ? true : false;
}

inline bool IsSpaceChar(int c)
{
	return c==' ' ? true : false;
}

typedef JAutoBuf<TCHAR, sizeof(TCHAR), 1> AutoTCHARs;
