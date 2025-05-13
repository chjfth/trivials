#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

extern HINSTANCE g_hinstExe;

#define ASSERT_DLG_SUCC(hdlg, dlg_rcid) \
	if((hdlg)==NULL) { \
		vaMsgBox(NULL, MB_OK|MB_ICONERROR, NULL, _T("Dialog creation fail for this dialogbox resource-id: ") _T(#dlg_rcid)); \
		exit(4); \
	}
