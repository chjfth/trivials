// A program to probe various Windows API returned buffer-size behavior,
// especially when user buffer is too small.
// This program prints a CSV file summarizing each WinAPI's traits based on live system run.
//
// Author: Jimm Chen, 2021.10 ~ 2025.02

#include <stdio.h>
#include <Userenv.h>
#include <locale.h>
#include <assert.h>
#include <tchar.h>

#include "share.h"

#define EXE_VERSION "1.9.1"

#ifdef UNICODE
#define UNICODE_OR_ANSI "Unicode"
#else
#define UNICODE_OR_ANSI "ANSI"
#endif

enum 
{ 
	MaxAPI = 100,
	Traits_STRMAXLEN = 120,
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
	
	BSR_Usersize = 1,      // API feedback: how many TCHARs are filled to user buffer(counting NUL)
	
	BSR_UsersizeLess1 = 2, // API feedback: how many TCHARs are filled to user buffer(not counting NUL)
	
	BSR_Total = 3,    // API feedback: how many TCHARs buffer required to correct the error(not counting NUL)
		// -- This is C99 snprintf behavior, not seen on winapi
	
	BSR_TotalPlus1=4, // API feedback: how many TCHARs buffer required to correct the error(counting NUL)

	BSR_TotalPlus2more = 5, // returns more than required

	BSR_BUG = 7
};

enum BufSmallFill_et // User buffer filling trait when buffer small
{
	BSF_Pure_NUL = 0,  // fill NUL to userbuf[0]
	BSF_LeaveAsIs,     // userbuf[] is not touched at all
	BSF_PartialAndNUL, // userbuf[] is filled to its up-limit and final cell NUL
	BSF_PartialNoNUL,  // userbuf[] is filled to its up-limit and final cell is not NUL

	BSF_BUG
};

enum BufEnoughRet_et // Returned trait when buffer enough
{
	ReturnT = 0,      // Return exactly "total" size, the correct result's strlen().
	                  // "Total"(T) does not count trailing NUL. 
	ReturnTplus1 = 1, // Return T+1

	ReturnTplus2ormore = 2, // Return T+2, and userbuf[T+1] is fill with NUL as well, redundant

	ReturnBug = 4,
};

enum EdgeBehavior_et // not used yet.
{
	Edge_OK = 0,
	EdgeBug_FeedbackLen = 1,
	EdgeBug_OverflowNUL = 2
};

struct ApiTrait_et
{
	BufSmallRet_et bs_ret;
	DWORD bs_winerr;
	BufSmallFill_et bs_fill;
	BufEnoughRet_et good_ret;
	EdgeBehavior_et edgecase;
};

BufSmallRet_et BufSmallRet_conclude(int small_input, int small_feedback, const TCHAR *soutput)
{
	int total_len = (int)_tcslen(soutput);

	if(small_feedback==-1)
		return BSR_Neg1;

	if(small_feedback==total_len)
		return BSR_Total;
	
	if(small_feedback==total_len+1)
		return BSR_TotalPlus1;

	if(small_feedback>=total_len+2)
		return BSR_TotalPlus2more;

	if(small_feedback==small_input)
		return BSR_Usersize;

	if(small_input>1 && small_feedback==small_input-1)
		return BSR_UsersizeLess1;

	if(small_feedback==BSR_Zero)
		return BSR_Zero;

	return BSR_BUG;
}

BufSmallFill_et BufSmallFill_conclude(
	int user_size, // user provided small-buflen
	// int eret_size, // returned buffer size on small buffer(e: error)
	const TCHAR *ebuf, // output buffer on small-buflen
	const TCHAR *sbuf,  // success buffer content(as comparison base)
	bool *pOverflowBug
	)
{
	int total_len = STRLEN(sbuf);
	assert(user_size<=total_len);

	// Check whether beyond-buffer memory is tampered. If so, I find a Microsoft bug.
	*pOverflowBug = false;
	for(int i=user_size; i<MAX_PATH; i++)
	{
		if(ebuf[i]!=BADCHAR)
		{
			*pOverflowBug = true;
			break;
		}
	}

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
	int total_size = (int)_tcslen(soutput);

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
	int startlen = (int)_tcslen(userbuf);

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

void vaDbgOutput(const TCHAR *fmt, ...)
{
	TCHAR buf[4000] = {};
	va_list args;
	va_start(args, fmt);

	_vsntprintf_s(buf, _TRUNCATE, fmt, args);

	va_end(args);

	_sntprintf_s(buf, _TRUNCATE, _T("%s\n"), buf);

	OutputDebugString(buf);
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

	vaDbgOutput(_T("#%d,%s,%s"), g_nowcase, apiname, traits);
}

void ReportTraits(const TCHAR *apiname,
	int small_buflen, // user input small buflen, ie. an "error" size 

	int eret_len, // "error-case" ret-len, the size that may/could correct the error
	DWORD winerr,
	const TCHAR *eoutput, // "error-case" output, may contain partial content

	int sret_len, // "success-case" ret-size, when user provides enough buffer
	const TCHAR *soutput, // "success" output, full outcome text.

	int edgeret_len, // "edge" means: user input buflen is exactly 'Total', no room for NUL
	const TCHAR *edge_retbuf
	)
{
	assert(sret_len!=FORGOT_INIT);

	// assert(eret_len!=FORGOT_INIT);
	// assert(edgeret_len!=FORGOT_INIT);
	// -- Don't assert above two, so to detect small/edge-case WinAPI error.
	//    We capture such error on WinXP GetVolumePathNamesForVolumeNameA .

	TCHAR tbuf[Traits_STRMAXLEN] = _T("");

	ApiTrait_et t = {};

	
	// Check enough buffer len-feedback

	if(sret_len!=LEN_NO_REPORT)
	{
		t.good_ret = BufEnoughRet_conclude(sret_len, soutput);
		switch(t.good_ret)
		{
		case ReturnT:
			vacat(tbuf, _T("T")); break;
		case ReturnTplus1:
			vacat(tbuf, _T("T+1")); break;
		case ReturnTplus2ormore:
			vacat(tbuf, _T("T+2!")); break;
		default:
			vacat(tbuf, TS_BUG);
		}
	}
	else
		vacat(tbuf, _T("-"));

	vacat(tbuf, _T(","));


	// Check small-buffer len-feedback

	if(eret_len!=LEN_NO_REPORT)
	{
		t.bs_ret = BufSmallRet_conclude(small_buflen, eret_len, soutput);
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
		case BSR_TotalPlus2more:
			vacat(tbuf, _T("Total+2!")); break;
		default:
			vacat(tbuf, _T("[BUG]"));
		}
	}
	else
		vacat(tbuf, _T("-"));
	
	vacat(tbuf, _T(","));

	// Report WinError code

	t.bs_winerr = winerr;
	if(winerr>0)
		vacat(tbuf, _T("%d"), t.bs_winerr);
	else
		vacat(tbuf, _T("/"));

	vacat(tbuf, _T(","));

	// Check partial fill

	if(eret_len==LEN_NO_REPORT)
	{
		// For cases like CM_Get_Device_ID()
		eret_len = small_buflen;
	}

	bool isOverflowBug = false;
	t.bs_fill = BufSmallFill_conclude(small_buflen, eoutput, soutput, &isOverflowBug);
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
		vacat(tbuf, TS_BUG);
	}

	bool isOverflowNUL = (eoutput[small_buflen]=='\0');
	if(isOverflowNUL)
		vacat(tbuf, TS_OVERFLOW_NUL);
	else if(isOverflowBug)
		vacat(tbuf, TS_SMALL_BUFFER_OVERFLOW);
	
	vacat(tbuf, _T(","));

	
	// Edge case check ...

	if(edge_retbuf!=NULL)
	{
		// ==== Check edge case (check API buggy/inconsistency behavior) ====
		// (1) BufSmallRet_et behavior should be consistent with small_buflen.
		// (2) edge_retbuf[sret_size] must not be filled with NUL after API call. 
		//     Note: ReportTraits's caller should have filled edge_retbuf[sret_size] with BADCHAR.
		// (3) Whether smallbuf-fill behavior remains consistent.

		TCHAR szEdgeBug[40] = {};

		int total_len = STRLEN(soutput);

		if(edgeret_len!=LEN_NO_REPORT)
		{
			BufSmallRet_et edgeret = BufSmallRet_conclude(total_len, edgeret_len, soutput);
			if(edgeret!=t.bs_ret)
			{
				if(edgeret==BSR_Total && total_len==edgeret_len)
					; // OK
				else
					vacat(szEdgeBug, _T("[FeedbackLen]"));
			}
		}

		BufSmallFill_et edgefill = BufSmallFill_conclude(total_len, edge_retbuf, soutput, &isOverflowBug);
		if(edgefill!=t.bs_fill)
		{
			vacat(szEdgeBug, TS_WACKY_FILL);
			// -- See comments in see_GetSystemDefaultLocaleName()
		}

		isOverflowNUL = (edge_retbuf[total_len]=='\0');
		if(isOverflowNUL) 
			vacat(szEdgeBug, TS_OVERFLOW_NUL);
		else if(isOverflowBug)
			vacat(szEdgeBug, TS_EDGE_BUFFER_OVERFLOW);

		///

		if(szEdgeBug[0]=='\0')
			vacat(szEdgeBug, TS_OK);

		vacat(tbuf, _T("%s"), szEdgeBug);
	}
	else
	{
		vacat(tbuf, TS_SKIP);
	}

	Add1Case(apiname, tbuf);
}

//////////////////////////////////////////////////////////////////////////

enum Sort_et { NoSort, DoSort, ReverseSort };

int __cdecl CompareTraits(void *ctx, const void *t1, const void *t2)
{
	Sort_et howsort = *(Sort_et*)ctx;
	ApiCase_st &c1 = *(ApiCase_st*)t1;
	ApiCase_st &c2 = *(ApiCase_st*)t2;
	
	int cmpret = _tcscmp(c1.traits, c2.traits);
	if(cmpret==0)
	{
		// if Traits are same, we go on compare API-name
		cmpret = _tcscmp(c1.apiname, c2.apiname);
	}
	return howsort==DoSort ? cmpret : -cmpret;
}

int _tmain(int argc, _TCHAR* argv[])
{
	setlocale(LC_ALL, ""); // only for printf Chinese chars purpose
	Prn(_T("seeRetBuf,version %s (%s)\n"), _T(EXE_VERSION), _T(UNICODE_OR_ANSI));
	Prn(_T("WinVer: %s\n"), app_GetWindowsVersionStr3());

	Sort_et howsort = NoSort;

	if(argc>1)
	{
		if( _tcscmp(argv[1], _T("sort"))==0 )
			howsort = DoSort;
		else if( _tcscmp(argv[1], _T("rsort"))==0 )
			howsort = ReverseSort;
	}

	check_apis();

	if(howsort==NoSort)
	{
		Prn(_T("#,WinAPI,GoodRet,BufSmallRet,WinErr,BufSmallFill,EdgeCaseBug\n"));

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
		qsort_s(gar_apicases, g_nowcase, sizeof(ApiCase_st), CompareTraits, &howsort);

		Prn(_T("API#,Behavior#,WinAPI,GoodRet,BufSmallRet,WinErr,BufSmallFill,EdgeCaseBug\n"));

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
