#ifndef __julGroupbox_iversion_h_
#define __julGroupbox_iversion_h_

#define julGroupbox_VMAJOR 1
#define julGroupbox_VMINOR 0
#define julGroupbox_VPATCH 0
#define julGroupbox_VTAIL  1

#define julGroupboxstr__(n) #n
#define julGroupboxstr(n) julGroupboxstr__(n)

// The following 4 are used in .rc
#define julGroupbox_VMAJORs julGroupboxstr(julGroupbox_VMAJOR)
#define julGroupbox_VMINORs julGroupboxstr(julGroupbox_VMINOR)
#define julGroupbox_VPATCHs julGroupboxstr(julGroupbox_VPATCH)
#define julGroupbox_VTAILs  julGroupboxstr(julGroupbox_VTAIL)

#define julGroupbox_NAME "julGroupbox"

enum {
	julGroupbox_vmajor = julGroupbox_VMAJOR,
	julGroupbox_vminor = julGroupbox_VMINOR,
	julGroupbox_vpatch = julGroupbox_VPATCH,
	julGroupbox_vtail = julGroupbox_VTAIL,
};


#endif
