#include "dlgbox-helper.h"

#include <assert.h>

struct DLGTEMPLATEEX
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;
/*
	sz_Or_Ord menu;
	sz_Or_Ord windowClass;
	WCHAR title[titleLen];
	WORD pointsize;
	WORD weight;
	BYTE italic;
	BYTE charset;
	WCHAR typeface[stringLen];
*/
};


DLGTEMPLATE* 
LoadAndPatchDialogTemplate(HINSTANCE hInst, LPCTSTR lpDialogName, WORD newPointSize)
{
	// With the help of ChatGPT.

    HRSRC hRes = FindResource(hInst, lpDialogName, RT_DIALOG);
    if (!hRes) 
		return NULL;

    DWORD size = SizeofResource(hInst, hRes);
    if (!size) 
		return NULL;

    HGLOBAL hGlob = LoadResource(hInst, hRes);
    if (!hGlob) 
		return NULL;

    DLGTEMPLATEEX* dlgEx = (DLGTEMPLATEEX*)LockResource(hGlob);

    BOOL isEx = (dlgEx->signature == 0xFFFF);
	assert(isEx); // The resource definition should be DIALOGEX, not DIALOG
	if (!isEx)
		return NULL;

    // Skip DLGTEMPLATEEX structure
	BYTE *p = (BYTE*)dlgEx;
	p += sizeof(DLGTEMPLATEEX);

    // Skip dlgbox-menu, dlgbox-class, window-title
    for (int i = 0; i < 3; ++i) {
        if (*(WORD*)p == 0x0000)
            p += 2;
        else if (*(WORD*)p == 0xFFFF)
            p += 4;
        else {
            while (*(WORD*)p) 
				p += 2;
        }
    }

    // Now we're at the font size (WORD), font weight (WORD), italic (BYTE), charset (BYTE), then font name (WCHAR[])
    WORD* pSize = (WORD*)p;
    *pSize++ = newPointSize;    // Font size

	pSize += 2;

    // Now pSize points to Unicode Font name, "MS Shell Dlg" etc

    return (DLGTEMPLATE*)dlgEx;
}
