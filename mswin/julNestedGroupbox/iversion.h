#ifndef __julNestedGroupbox_iversion_h_
#define __julNestedGroupbox_iversion_h_

#define julNestedGroupbox_VMAJOR 1
#define julNestedGroupbox_VMINOR 0
#define julNestedGroupbox_VPATCH 0
#define julNestedGroupbox_VTAIL  1

#define julNestedGroupboxstr__(n) #n
#define julNestedGroupboxstr(n) julNestedGroupboxstr__(n)

// The following 4 are used in .rc
#define julNestedGroupbox_VMAJORs julNestedGroupboxstr(julNestedGroupbox_VMAJOR)
#define julNestedGroupbox_VMINORs julNestedGroupboxstr(julNestedGroupbox_VMINOR)
#define julNestedGroupbox_VPATCHs julNestedGroupboxstr(julNestedGroupbox_VPATCH)
#define julNestedGroupbox_VTAILs  julNestedGroupboxstr(julNestedGroupbox_VTAIL)

#define julNestedGroupbox_NAME "julNestedGroupbox"

enum {
	julNestedGroupbox_vmajor = julNestedGroupbox_VMAJOR,
	julNestedGroupbox_vminor = julNestedGroupbox_VMINOR,
	julNestedGroupbox_vpatch = julNestedGroupbox_VPATCH,
	julNestedGroupbox_vtail = julNestedGroupbox_VTAIL,
};


#endif
