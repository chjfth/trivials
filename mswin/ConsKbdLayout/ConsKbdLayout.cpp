#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <assert.h>
#include <conio.h>
#include <windows.h>

void test_kbdlayout()
{
	_tprintf(_T("GetKeyboardLayoutList() returns following list:\n"));

	HKL curhkl = GetKeyboardLayout(0);

	HKL arHkl[100] = {};
	int nkl = GetKeyboardLayoutList(ARRAYSIZE(arHkl), arHkl);
	assert(nkl>0);

	int i;
	for(i=0; i<nkl; i++)
	{
		HKL hkl = arHkl[i];
		LANGID langid = LOWORD(hkl);

		TCHAR langtag[LOCALE_NAME_MAX_LENGTH+1] = {};
		LCIDToLocaleName(langid, langtag, LOCALE_NAME_MAX_LENGTH, 0);
		
		TCHAR langname[80] = {};
		GetLocaleInfo(langid, LOCALE_SENGLISHDISPLAYNAME, langname, ARRAYSIZE(langname));

		_tprintf(_T("%c[#%-2d] 0x%04X.%04X : %s, %s\n"),
			hkl==curhkl ? '*' : ' ',
			i,
			HIWORD(hkl), langid, 
			langtag, langname
			);
	}

	_tprintf(_T("\n"));
	_tprintf(_T("Select one to activate: "));

	TCHAR inbuf[80];
	_fgetts(inbuf, ARRAYSIZE(inbuf), stdin);

	int sel = _ttoi(inbuf);
	_tprintf(_T("You selected: #%d\n"), sel);

	HKL newhkl = arHkl[sel];

	HKL prevhkl = ActivateKeyboardLayout(newhkl, KLF_SETFORPROCESS);
	if(prevhkl!=0)
	{
		_tprintf(_T("ActivateKeyboardLayout(0x%08X) success.\n"), newhkl);

		// But, it looks like ActivateKeyboardLayout() does not actually take effect
		// for a console exe.
	}
	else
	{
		DWORD winerr = GetLastError();
		_tprintf(_T("ActivateKeyboardLayout(0x%08X) fail, winerr=%d\n"), newhkl, winerr);
	}

	_tprintf(_T("Press Enter to quit.\n"));
	_fgetts(inbuf, ARRAYSIZE(inbuf), stdin);
}

int _tmain(int argc, TCHAR* argv[])
{
	setlocale(LC_ALL, "");
	
	test_kbdlayout();

	return 0;
}

