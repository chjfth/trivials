#pragma once
#include <windows.h>

enum DlgboxCbw_err 
{
	DlgboxCbw_Succ = 0,

	// Error on enable
	DlgboxCbw_Unknown = 1,
	DlgboxCbw_NoMem = 2,
	DlgboxCbw_AlreadyEnabled = 3,
	DlgboxCbw_SetProp = 4, // WinAPI SetProp() error

	// Error on disable
	DlgboxCbw_NotEnabledYet = 10,
	DlgboxCbw_ChainMoved = 11,
};

DlgboxCbw_err Dlgbox_EnableComboboxWideDrop(HWND hdlg);

DlgboxCbw_err Dlgbox_DisableComboboxWideDrop(HWND hdlg);

