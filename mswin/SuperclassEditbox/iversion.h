#ifndef __SuperclassEditbox_iversion_h_
#define __SuperclassEditbox_iversion_h_

#define SuperclassEditbox_VMAJOR 1
#define SuperclassEditbox_VMINOR 0
#define SuperclassEditbox_VPATCH 0
#define SuperclassEditbox_VTAIL  1

#define SuperclassEditboxstr__(n) #n
#define SuperclassEditboxstr(n) SuperclassEditboxstr__(n)

// The following 4 are used in .rc
#define SuperclassEditbox_VMAJORs SuperclassEditboxstr(SuperclassEditbox_VMAJOR)
#define SuperclassEditbox_VMINORs SuperclassEditboxstr(SuperclassEditbox_VMINOR)
#define SuperclassEditbox_VPATCHs SuperclassEditboxstr(SuperclassEditbox_VPATCH)
#define SuperclassEditbox_VTAILs  SuperclassEditboxstr(SuperclassEditbox_VTAIL)

#define SuperclassEditbox_NAME "SuperclassEditbox"

enum {
	SuperclassEditbox_vmajor = SuperclassEditbox_VMAJOR,
	SuperclassEditbox_vminor = SuperclassEditbox_VMINOR,
	SuperclassEditbox_vpatch = SuperclassEditbox_VPATCH,
	SuperclassEditbox_vtail = SuperclassEditbox_VTAIL,
};


#endif
