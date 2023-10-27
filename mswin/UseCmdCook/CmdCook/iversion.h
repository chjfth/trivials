#ifndef __CmdCook_iversion_h_
#define __CmdCook_iversion_h_

// This iversion.h is considered internal to CmdCook implementation.
// API user should call CmdCook_getversion() to query its version at runtime.

#define THISLIB_VMAJOR 1
#define THISLIB_VMINOR 0
#define THISLIB_VPATCH 0
#define THISLIB_VEXTRA 0

#define THISLIBstr__(n) #n
#define THISLIBstr(n) THISLIBstr__(n)

// The following 4 are used in .rc, 's' implies string.
#define THISLIB_VMAJORs THISLIBstr(THISLIB_VMAJOR)
#define THISLIB_VMINORs THISLIBstr(THISLIB_VMINOR)
#define THISLIB_VPATCHs THISLIBstr(THISLIB_VPATCH)
#define THISLIB_VEXTRAs THISLIBstr(THISLIB_VEXTRA)

#define THISLIB_NAME "CmdCook"

enum {
	CmdCook_vmajor = THISLIB_VMAJOR,
	CmdCook_vminor = THISLIB_VMINOR,
	CmdCook_vpatch = THISLIB_VPATCH,
	CmdCook_vextra = THISLIB_VEXTRA,
};


#endif
