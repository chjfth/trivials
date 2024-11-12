#ifndef __MyDLL2_iversion_h_
#define __MyDLL2_iversion_h_

// This iversion.h is considered internal to MyDLL2 implementation.
// API user should call MyDLL2_getversion() to query its version at runtime.

#define THISLIB_VMAJOR 1
#define THISLIB_VMINOR 0
#define THISLIB_VPATCH 0
#define THISLIB_VEXTRA 2

#define THISLIBstr__(n) #n
#define THISLIBstr(n) THISLIBstr__(n)

// The following 4 are used in .rc, 's' implies string.
#define THISLIB_VMAJORs THISLIBstr(THISLIB_VMAJOR)
#define THISLIB_VMINORs THISLIBstr(THISLIB_VMINOR)
#define THISLIB_VPATCHs THISLIBstr(THISLIB_VPATCH)
#define THISLIB_VEXTRAs THISLIBstr(THISLIB_VEXTRA)

#define THISLIB_NAME "MyDLL2"

enum {
	MyDLL2_vmajor = THISLIB_VMAJOR,
	MyDLL2_vminor = THISLIB_VMINOR,
	MyDLL2_vpatch = THISLIB_VPATCH,
	MyDLL2_vextra = THISLIB_VEXTRA,
};


#endif
