#pragma once

#ifdef UNICODE
#define Memset wmemset
#else
#define Memset memset
#endif

#define NULCHAR _T('\0')
#define BADCHAR _T('*')

void ReportTraits(const TCHAR *apiname,
	int user_size, int eret_size, DWORD winerr,
	const TCHAR *eoutput, 
	int sret_size, const TCHAR *soutput);

void check_apis();

