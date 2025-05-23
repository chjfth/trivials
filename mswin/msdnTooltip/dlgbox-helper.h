#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdint.h>

DLGTEMPLATE* LoadAndPatchDialogTemplate(HINSTANCE hInst, LPCTSTR lpDialogName, 
	WORD newPointSize);

