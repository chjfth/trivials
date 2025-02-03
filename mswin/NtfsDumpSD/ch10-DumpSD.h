#pragma once

#include <EnsureClnup_mswin.h>
MakeCleanupPtrClass_winapi(Cec_LocalFree, HLOCAL, LocalFree, HLOCAL)

extern TCHAR g_dbgbuf[];

typedef TCHAR* FUNC_InterpretRights(DWORD RightBits, void *userctx);
// -- RightBits refers to ACCESS_ALLOWED_ACE.Mask
//    The returned string pointer should be C++-deleted by caller

void CH10_DumpSD(PSECURITY_DESCRIPTOR pvsd, 
	FUNC_InterpretRights *procItr=nullptr, void *userctx=nullptr);


