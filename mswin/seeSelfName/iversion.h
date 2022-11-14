#ifndef __seeSelfName_iversion_h_
#define __seeSelfName_iversion_h_


#define seeSelfName_VMAJOR 1
#define seeSelfName_VMINOR 0
#define seeSelfName_VPATCH 0
#define seeSelfName_VTAIL  1

#define seeSelfNamestr__(n) #n
#define seeSelfNamestr(n) seeSelfNamestr__(n)

// The following 4 are used in .rc
#define seeSelfName_VMAJORs seeSelfNamestr(seeSelfName_VMAJOR)
#define seeSelfName_VMINORs seeSelfNamestr(seeSelfName_VMINOR)
#define seeSelfName_VPATCHs seeSelfNamestr(seeSelfName_VPATCH)
#define seeSelfName_VTAILs  seeSelfNamestr(seeSelfName_VTAIL)

#define seeSelfName_NAME "seeSelfName"

enum {
	seeSelfName_vmajor = seeSelfName_VMAJOR,
	seeSelfName_vminor = seeSelfName_VMINOR,
	seeSelfName_vpatch = seeSelfName_VPATCH,
	seeSelfName_vtail = seeSelfName_VTAIL,
};


#endif
