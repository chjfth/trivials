#include <stdio.h>
#include <stdlib.h>
#include <ps_TCHAR.h>
#include "t_english_dict.h"

#ifdef __linux_
#include <linux/msvc_extras.h>
#endif

#undef NDEBUG // Enable assert for Release-build

#include <CHHI_vaDBG_is_vaDbgTs.h> // optional

#include <hashdict.h>
using namespace chjds;

bool t_english_dict(const TCHAR* dictfilename)
{
	// Note: Words in dictfilename must not duplicate.

	return true;
}
