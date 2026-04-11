#include <stdio.h>
#include <stdlib.h>
#include <ps_TCHAR.h>
#include <vaDbgTs.h>
#include <fsapi.h>
	using namespace fsapi;
#include <makeTsdring.h>
#include <StringHelper.h>
#include <EnsureClnup_misc.h>
#include <msvc_extras.h>

#include "CHHI_DEBUG.h"

#undef NDEBUG // Enable assert for Release-build

#include <CHHI_vaDBG_is_vaDbgTs.h> // optional

#include <hashdict.h>
using namespace chjds;


#include "t_english_dict.h"


inline bool StringSplitter_IsCrlfNul(int charval)
{
	return (charval=='\r' || charval=='\n' || charval=='\0') ? true : false;
}


bool t_english_dict(const TCHAR* dictfilename, int resize_pct)
{
	// Note: Duplicate words in dictfilename will be considered as one single word.

	CEC_filehandle_t fh = file_open(dictfilename, open_for_read, open_share_read);
	if(!fh)
	{
		_tprintf(_T("Error: Cannot open file '%s'\n"), dictfilename);
		return false;
	}

	int filelen = (int)file_getsize(fh);
	if(filelen==0)
	{
		_tprintf(_T("Error: File is zero size: '%s'\n"), dictfilename);
		return false;
	}

	sdring<char> filebin(filelen);
	filebin[filelen] = '\0'; // byte stream

	int bytesRed = file_read(fh, filebin, filelen);
	assert(bytesRed==filelen);

	Sdring alltext = makeTsdring_UTF8(std::move(filebin));

	_tprintf(_T("Dict file loaded: '%s'\n"), dictfilename);

	// Define a hashdict and inject dict words into it.
	hashdict<int> dict1(_T("D1"));
	dict1.SetDbgParams(resize_pct);

	StringSplitter<TCHAR*, StringSplitter_IsCrlfNul> sp(alltext); // ok with 16:40

	Uint msec_start = va_millisec();

	int i=0, total_words = 0;
	int len = 0;
	int dups = 0;
	
	for(;; i++)
	{
		int pos = sp.next(&len);
		if(pos==-1)
			break;

		// make the sub-string NUL-terminated for easier later processing
		TCHAR *nowword = &alltext[pos];
		nowword[len] = '\0';

		int iline = i+1; // iline is one-based
//		 _tprintf(_T("[#%d] %s\n"), iline, &alltext[pos]);
		
		int *pwas = dict1.setdefault(nowword, iline);
		if(*pwas==iline)
		{
			total_words++;
		}
		else
		{
			_tprintf(_T("Input file error: Found duplicate word '%s', dup at line#%d, first seen on line#%d\n"),
				nowword, iline, *pwas);
			dups++;
		}
	}

	Uint msec_end = va_millisec();

	_tprintf(_T("Hashdict build done, cost %u millisec\n"), msec_end-msec_start);

	_tprintf(_T("Total non-duplicate words: %d\n"), total_words);

	if(dups>0)
	{
		_tprintf(_T("Duplicate words(total %d) found in dictfile, I cannot continue.\n"), dups);
		return false;
	}

	//
	// Check all words existence from dict.
	//
	sp.reset();
	msec_start = va_millisec();

	for(i=0;; i++)
	{
		int pos = sp.next(&len);
		if(pos==-1)
			break;

		const TCHAR *nowword = &alltext[pos];
		int *pline = dict1.get(nowword);

		int iline = i+1;
		assert(*pline==iline);
	}

	msec_end = va_millisec();

	_tprintf(_T("[DictTest1] Whole dict look up costs %u millisec.\n"), msec_end-msec_start);

	//
	// Check some non-existence words.
	//

	const TCHAR *const alienwords[] = {_T("alien1"), _T("z0xyz"), _T("A.0.B") };
	for(i=0; i<ARRAYSIZE(alienwords); i++)
	{
		int *pline = dict1.get(alienwords[i]);
		assert(pline==nullptr);
	}
	_tprintf(_T("[DictTest2] Alien words look-up OK.\n"));

	//
	// Move half words from dict1 to new dict2
	//

	hashdict<int> dict2(_T("D2"));
	dict2.SetDbgParams(resize_pct);

	sp.reset();
	msec_start = va_millisec();
	int dict1_words = total_words, dict2_words = 0;

	for(i=0;; i++)
	{
		int pos = sp.next(&len);
		if(pos==-1)
			break;

		const TCHAR *nowword = &alltext[pos];
		int iline = i+1;

		if(i%2 == 1)
		{
			int oldval = -1;
			dict1.del(nowword, oldval);
			assert(oldval = iline);
			dict1_words--;

			int *pline = dict2.set(nowword, oldval);
			assert(*pline == oldval);
			dict2_words++;
		}
	}

	assert( abs(dict1_words-dict2_words) <= 1 );

	// Do verify
	sp.reset();
	for(i=0;; i++)
	{
		int pos = sp.next(&len);
		if(pos==-1)
			break;

		const TCHAR *nowword = &alltext[pos];
		int iline = i+1;

		int *pi1 = dict1.get(nowword);
		int *pi2 = dict2.get(nowword);

		if(i%2 == 0)
		{
			assert(*pi1==iline);
			assert(!pi2);
		}
		else
		{
			assert(!pi1);
			assert(*pi2==iline);
		}
	}

	msec_end = va_millisec();

	_tprintf(_T("[DictTest3] Split dict into halves and verify, costs %d millisec.\n"), msec_end-msec_start);

	//
	// Move words from dict2 back into dict1. We should see dict2 shrunk.
	//

	hashdict<int>::ReportState_st st1a={sizeof(st1a)}, st1b={sizeof(st1b)};
	hashdict<int>::ReportState_st st2a={sizeof(st2a)}, st2b={sizeof(st2b)};
	dict1.ReportState(&st1a); 
	dict2.ReportState(&st2a); 

	msec_start = va_millisec();

	const TCHAR *key = nullptr;
	Sdring keydel;

	hashdict<int>::enumor en2(dict2);

	int ndels = 0;
	for(;; ndels++)
	{
		int *pvalue = nullptr;
		const TCHAR *key = en2.next(&pvalue);
		keydel = key; // make a copy of the key str

		if(keydel)
		{
			int oldval = -1;
			bool succ = dict2.del(key, oldval);
			assert(succ);

			// We must use `keydel` below, bcz `key` has become a dangling pointer after `del()`.
			int *pi = dict1.setdefault(keydel, oldval);
			assert(*pi==oldval);
		}
		else
			break;
	}

	assert(ndels==dict2_words);
	assert(ndels+dict1_words == total_words);

	dict1.ReportState(&st1b); 
	dict2.ReportState(&st2b); 

	// Verify dict1 content again.

	sp.reset();
	for(i=0;; i++)
	{
		int pos = sp.next(&len);
		if(pos==-1)
			break;

		const TCHAR *nowword = &alltext[pos];
		int *pline = dict1.get(nowword);

		int iline = i+1;
		assert(*pline==iline);
	}

	assert(i==total_words);

	msec_end = va_millisec();

	_tprintf(_T("[DictTest4] Merge dict2 back into dict1 and verify, costs %d millisec.\n"), msec_end-msec_start);
	_tprintf(_T("            dict1 memuse increased from %lld to %lld (diff %lld bytes)\n"), 
		st1a.total_memuse64, st1b.total_memuse64,   st1b.total_memuse64-st1a.total_memuse64);
	_tprintf(_T("            dict2 memuse reduced from %lld to %lld (diff %lld bytes)\n"), 
		st2a.total_memuse64, st2b.total_memuse64,   st2b.total_memuse64-st2a.total_memuse64);
	assert(st1a.total_memuse64 < st1b.total_memuse64);
	assert(st2a.total_memuse64 > st2b.total_memuse64);

	//
	// Test that dict2 adding key is OK after shrinking
	//
	sp.reset();
	for(i=0;; i++)
	{
		int pos = sp.next(&len);
		if(pos==-1)
			break;

		int iline = i+1;
		const TCHAR *nowword = &alltext[pos];
		int *pline = dict1.set(nowword, iline);
		assert(*pline==iline);
	}

	return true;
}
