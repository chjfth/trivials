#ifndef __tCxxWindowSubclass_iversion_h_
#define __tCxxWindowSubclass_iversion_h_

#define tCxxWindowSubclass_VMAJOR 1
#define tCxxWindowSubclass_VMINOR 1
#define tCxxWindowSubclass_VPATCH 1
#define tCxxWindowSubclass_VTAIL  0

#define tCxxWindowSubclassstr__(n) #n
#define tCxxWindowSubclassstr(n) tCxxWindowSubclassstr__(n)

// The following 4 are used in .rc
#define tCxxWindowSubclass_VMAJORs tCxxWindowSubclassstr(tCxxWindowSubclass_VMAJOR)
#define tCxxWindowSubclass_VMINORs tCxxWindowSubclassstr(tCxxWindowSubclass_VMINOR)
#define tCxxWindowSubclass_VPATCHs tCxxWindowSubclassstr(tCxxWindowSubclass_VPATCH)
#define tCxxWindowSubclass_VTAILs  tCxxWindowSubclassstr(tCxxWindowSubclass_VTAIL)

#define tCxxWindowSubclass_NAME "tCxxWindowSubclass"

enum {
	tCxxWindowSubclass_vmajor = tCxxWindowSubclass_VMAJOR,
	tCxxWindowSubclass_vminor = tCxxWindowSubclass_VMINOR,
	tCxxWindowSubclass_vpatch = tCxxWindowSubclass_VPATCH,
	tCxxWindowSubclass_vtail = tCxxWindowSubclass_VTAIL,
};


#endif
