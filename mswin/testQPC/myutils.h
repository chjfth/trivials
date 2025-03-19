#pragma once
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>

inline TCHAR* BigNum64ToString(__int64 lNum, TCHAR szBuf[], int chBufSize)
{
	TCHAR szNum[100] = {};
	_sntprintf_s(szNum, _TRUNCATE, TEXT("%I64d"), lNum);
	NUMBERFMT nf = {};
	nf.NumDigits = 0;
	nf.LeadingZero = FALSE;
	nf.Grouping = 3;
	nf.lpDecimalSep = TEXT(".");
	nf.lpThousandSep = TEXT(",");
	nf.NegativeOrder = 0;
	GetNumberFormat(LOCALE_USER_DEFAULT, 0, szNum, &nf, szBuf, chBufSize);
	return(szBuf);
}
