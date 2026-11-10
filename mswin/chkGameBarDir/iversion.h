#ifndef __chkGameBarDir_iversion_h_
#define __chkGameBarDir_iversion_h_

#define chkGameBarDir_VMAJOR 1
#define chkGameBarDir_VMINOR 0
#define chkGameBarDir_VPATCH 0
#define chkGameBarDir_VTAIL  1

#define chkGameBarDirstr__(n) #n
#define chkGameBarDirstr(n) chkGameBarDirstr__(n)

// The following 4 are used in .rc
#define chkGameBarDir_VMAJORs chkGameBarDirstr(chkGameBarDir_VMAJOR)
#define chkGameBarDir_VMINORs chkGameBarDirstr(chkGameBarDir_VMINOR)
#define chkGameBarDir_VPATCHs chkGameBarDirstr(chkGameBarDir_VPATCH)
#define chkGameBarDir_VTAILs  chkGameBarDirstr(chkGameBarDir_VTAIL)

#define chkGameBarDir_NAME "chkGameBarDir"

enum {
	chkGameBarDir_vmajor = chkGameBarDir_VMAJOR,
	chkGameBarDir_vminor = chkGameBarDir_VMINOR,
	chkGameBarDir_vpatch = chkGameBarDir_VPATCH,
	chkGameBarDir_vtail = chkGameBarDir_VTAIL,
};


#endif
