// A program to probe various Windows API returned buffer-size behavior,
// especially when user buffer is too small.
// This program prints a CSV file summarizing each WinAPI's traits based on live system run.
//
// Author: Jimm Chen, 2021.10
//
// Compile the program with:
//
//    cl /DUNICODE /D_UNICODE seeRetBuf.cpp checkapis.cpp /link user32.lib psapi.lib userenv.lib gdi32.lib ole32.lib SetupAPI.lib

#include <stdio.h>
#include <Userenv.h>
#include <locale.h>
#include <assert.h>
#include <tchar.h>

#include "share.h"

enum 
{ 
	MaxAPI = 100,
	Traits_STRMAXLEN = 60,
};

struct ApiCase_st
{
	const TCHAR *apiname;
	TCHAR traits[Traits_STRMAXLEN];  // e.g: "Total+1,0,No*,T"
};

ApiCase_st gar_apicases[MaxAPI];
int g_nowcase = 0; // How many elements in gar_apicases[MaxAPI]


enum BufSmallRet_et // Ret trait when buffer is too small
{
	BSR_Neg1 = -1,
	BSR_Zero = 0,
	BSR_Usersize,
	BSR_UsersizeLess1,
	BSR_Total, // C99 snprintf behavior, not seen on winapi
	BSR_TotalPlus1,

	BSR_BUG
};

enum BufSmallFill_et // User buffer filling trait when buffer small
{
	BSF_Pure_NUL = 0,  // fill NUL to userbuf[0]
	BSF_LeaveAsIs,     // userbuf[] is not touched at all
	BSF_PartialAndNUL, // userbuf[] is filled to its up-limit and final cell NUL
	BSF_PartialNoNUL,  // userbuf[] is filled to its up-limit and final cell is not NUL

	BSF_BUG
};

enum BufEnoughRet_et // Ret trait when buffer enough
{
	ReturnT = 0,      // Return exactly "total" size, the correct result's strlen().
	                  // "Total"(T) does not count trailing NUL. 
	ReturnTplus1 = 1, // Return T+1

	ReturnTplus2ormore = 2, // Return T+2, and userbuf[T+1] is fill with NUL as well, redundant

	ReturnBug = 4,
};

struct ApiTrait_et
{
	BufSmallRet_et bs_ret;
	DWORD bs_winerr;
	BufSmallFill_et bs_fill;
	BufEnoughRet_et good_ret;
};

BufSmallRet_et BufSmallRet_conclude(int user_size, int eret_size, const TCHAR *soutput)
{
	int total_size = _tcslen(soutput);

	if(eret_size==-1)
		return BSR_Neg1;

	if(eret_size==total_size)
		return BSR_Total;
	
	if(eret_size==total_size+1)
		return BSR_TotalPlus1;

	if(eret_size==user_size)
		return BSR_Usersize;

	if(user_size>1 && eret_size==user_size-1)
		return BSR_UsersizeLess1;

	if(eret_size==BSR_Zero)
		return BSR_Zero;

	return BSR_BUG;
}

BufSmallFill_et BufSmallFill_conclude(
	int user_size, // user input buffer size
	int eret_size, // returned buffer size on small buffer(e: error)
	const TCHAR *ebuf, const TCHAR *sbuf)
{
	if(ebuf[0]==NULCHAR)
		return BSF_Pure_NUL;

	if(ebuf[0]==BADCHAR && ebuf[1]==BADCHAR)
		return BSF_LeaveAsIs;

	if(_tcsncmp(ebuf, sbuf, user_size-1)==0)
	{
		if( ebuf[user_size-1]==NULCHAR )
			return BSF_PartialAndNUL;
		else if ( ebuf[user_size-1]==sbuf[user_size-1] )
		{
			return BSF_PartialNoNUL;
		}
		else if ( ebuf[user_size-1]==BADCHAR )
		{
			// This is special wacky behavior of GetDefaultUserProfileDirectory().
			// Microsoft programmer forgot to fill the final char.
			return BSF_PartialNoNUL;
		}
		else
			return BSF_BUG;
	}
	
	return BSF_BUG;
}

bool IsAllNUL(const TCHAR *p, int n)
{
	for(int i=0; i<n; i++)
	{
		if(p[i]!=NULCHAR)
			return false;
	}
	return true;
}

BufEnoughRet_et BufEnoughRet_conclude(
	int sret_size, // returned buffer size on success
	const TCHAR *soutput // output buffer of success case
	)
{
	int total_size = _tcslen(soutput);

	if(sret_size==total_size)
		return ReturnT;

	if(sret_size==total_size+1)
		return ReturnTplus1;

	if(sret_size>total_size+1 
		&& IsAllNUL(soutput+total_size+1, sret_size-total_size-1))
		return ReturnTplus2ormore;

	return ReturnBug;
}

template <size_t bufchars>
void vacat(
	TCHAR (&userbuf)[bufchars],
	const TCHAR *fmt, ...)
{
	int startlen = _tcslen(userbuf);

	va_list args;
	va_start(args, fmt);
	
	_vsntprintf_s(userbuf+startlen, bufchars-startlen, _TRUNCATE, fmt, args);

	va_end(args);
}

void Prn(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	_vtprintf(fmt, args);

	va_end(args);
}

void Add1Case(const TCHAR *apiname, const TCHAR *traits)
{
	int n = g_nowcase;
	
	if(n==MaxAPI)
	{
		_tprintf(_T("Error! Reached MaxAPI.\n"));
		exit(4);
	}

	gar_apicases[n].apiname = apiname;
	_tcscpy_s(gar_apicases[n].traits, traits);

	g_nowcase++;
}

void ReportTraits(const TCHAR *apiname,
	int user_size, int eret_size, DWORD winerr,
	const TCHAR *eoutput, 
	int sret_size, const TCHAR *soutput)
{
	TCHAR tbuf[Traits_STRMAXLEN] = _T("");

	ApiTrait_et t = {};
	
	t.bs_ret = BufSmallRet_conclude(user_size, eret_size, soutput);
	switch(t.bs_ret)
	{
	case BSR_Neg1: 
		vacat(tbuf, _T("-1")); break;
	case BSR_Zero: 
		vacat(tbuf, _T("Zero")); break;
	case BSR_Usersize: 
		vacat(tbuf, _T("Usersize")); break;
	case BSR_UsersizeLess1:
		vacat(tbuf, _T("Usersize-1")); break;
	case BSR_Total:
		vacat(tbuf, _T("Total")); break;
	case BSR_TotalPlus1:
		vacat(tbuf, _T("Total+1")); break;
	default:
		vacat(tbuf, _T("[BUG]"));
	}
	
	vacat(tbuf, _T(","));

	t.bs_winerr = winerr;
	vacat(tbuf, _T("%d"), t.bs_winerr);

	vacat(tbuf, _T(","));

	t.bs_fill = BufSmallFill_conclude(user_size, eret_size, eoutput, soutput);
	switch(t.bs_fill)
	{
	case BSF_Pure_NUL:
		vacat(tbuf, _T("No+NUL")); break;
	case BSF_LeaveAsIs:
		vacat(tbuf, _T("No*")); break;
	case BSF_PartialAndNUL:
		vacat(tbuf, _T("Yes+NUL")); break;
	case BSF_PartialNoNUL:
		vacat(tbuf, _T("Yes*")); break;
	default:
		vacat(tbuf, _T("[BUG]"));
	}
	
	vacat(tbuf, _T(","));

	t.good_ret = BufEnoughRet_conclude(sret_size, soutput);
	switch(t.good_ret)
	{
	case ReturnT:
		vacat(tbuf, _T("T")); break;
	case ReturnTplus1:
		vacat(tbuf, _T("T+1")); break;
	case ReturnTplus2ormore:
		vacat(tbuf, _T("T+2!")); break;
	default:
		vacat(tbuf, _T("[BUG]"));
	}

	Add1Case(apiname, tbuf);
}

//////////////////////////////////////////////////////////////////////////


int __cdecl CompareTraits(void *ctx, const void *t1, const void *t2)
{
	ApiCase_st &c1 = *(ApiCase_st*)t1;
	ApiCase_st &c2 = *(ApiCase_st*)t2;
	return _tcscmp(c1.traits, c2.traits);
}

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, ""); // only for printf Chinese chars purpose

	bool is_sort = false;

	if(argc>1 && _tcscmp(argv[1], _T("sort"))==0)
	{
		is_sort = true;
	}

	check_apis();

	if(!is_sort)
	{
		Prn(_T("#,WinAPI,BufSmallRet,WinErr,BufSmallFill,GoodRet\n"));

		int i;
		for(i=0; i<g_nowcase; i++)
		{
			Prn(_T("%d,%s,%s\n"), i+1, gar_apicases[i].apiname, gar_apicases[i].traits);
		}

		Prn(_T("\n"));
		Prn(_T("Hint: Add parameter \"sort\" to count how many behaviors there are.\n"));
	}
	else
	{
		qsort_s(gar_apicases, g_nowcase, sizeof(ApiCase_st), CompareTraits, NULL);

		Prn(_T("API#,Behavior#,WinAPI,BufSmallRet,WinErr,BufSmallFill,GoodRet\n"));

		int behavior = 0, i;
		const TCHAR *prev_traits = _T("");
		for(i=0; i<g_nowcase; i++)
		{
			const ApiCase_st &curcase = gar_apicases[i];

			if(_tcscmp(prev_traits, curcase.traits)!=0)
			{
				behavior++;
				prev_traits = curcase.traits;
			}

			Prn(_T("%d,%d,%s,%s\n"), i+1, behavior, 
				gar_apicases[i].apiname, gar_apicases[i].traits);
		}
	}

	return 0;
}
