#undef NDEBUG // always enable assert()

// This dedicated CHHI_DEBUG.h ensures that all CHHI headers receive 
// these same macro values. Different CHHI debug macro values will
// cause different .cpp to see different C++ template code.

#define vaDBG_MAX_LEVEL 3 // use 3 to see most verbose vaDBG output

#define SimpleIni_DEBUG

