#include <windows.h>
#include "../utils.h" // provides vaDbgTs() declaration
#include <CHHI_vaDBG_is_vaDbgTs.h> // use vaDbgTs() inside

#define JAutoBuf_IMPL
#include <JAutoBuf.h>

#define WinMultiMon_IMPL
#define WinMultiMon_DEBUG
#include <mswin/WinMultiMon.h>

#define JULayout2_IMPL
#include <mswin/JULayout2.h>
