#if 0

// Actual developer can copy these to his own $prjdir/devdbg/CHHI_DEBUG.h 
// (except that change `#if 0` to `#if 1`).

#undef NDEBUG // always enable assert()

// This dedicated CHHI_DEBUG.h ensures that all CHHI headers receive 
// these same macro values. Different CHHI debug macro values will
// cause different .cpp to see different C++ template code.

#define vaDBG_MAX_LEVEL 2 // use 3 to see most verbose vaDBG output

#define DigClock2_DEBUG

#define Editbox_EnableKbdAdjustNumber_DEBUG
#define SimpleIni_DEBUG
#define SimpleIniEx_DEBUG
#define DataXIni_DEBUG

#endif
