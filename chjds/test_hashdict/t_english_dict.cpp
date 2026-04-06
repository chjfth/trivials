#include <stdio.h>
#include <stdlib.h>
#include <ps_TCHAR.h>
#include <fsapi.h>
	using namespace fsapi;
#include <makeTsdring.h>
#include <StringHelper.h>
#include "t_english_dict.h"

#if defined(__linux__) || defined(__CYGWIN__)
#include <linux/msvc_extras.h>
#else
#include <tchar.h>
#endif

#undef NDEBUG // Enable assert for Release-build

#include <CHHI_vaDBG_is_vaDbgTs.h> // optional

#include <hashdict.h>
using namespace chjds;

#include <EnsureClnup_misc.h>

bool t_english_dict(const TCHAR* dictfilename)
{
	// Note: Words in dictfilename must not duplicate.

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

	TCHAR *ws = alltext;
	StringSplitter<decltype(ws)> sp(ws);

	int i=0;
	for(;;i++)
	{
		int len = 0;
		int pos = sp.next(&len);
		if(pos==-1)
			break;

		_tprintf(_T("[#%d] %.*s\n"), i+1, len, &ws[pos]);
	}

	return true;
}
