#pragma once

#ifdef UNICODE
#define Memset wmemset
#else
#define Memset memset
#endif

#define NULCHAR _T('\0')
#define BADCHAR _T('*')

#define LEN_NO_REPORT (-2)  // some API does not report/feedback required buflen
#define FORGOT_INIT (-4)

#define STRLEN(s) (int)_tcslen(s)


void ReportTraits(const TCHAR *apiname,
	int small_buflen, int eret_size, DWORD winerr,
	const TCHAR *eoutput, 
	int sret_size, const TCHAR *soutput, 
	int edge_retsize=0, const TCHAR *edge_retbuf=NULL);

void check_apis();

void Prn(const TCHAR *fmt, ...);
