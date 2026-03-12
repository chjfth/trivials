#ifndef __Editbox_EnableKbdAdjust_iversion_h_
#define __Editbox_EnableKbdAdjust_iversion_h_

#define Editbox_EnableKbdAdjust_VMAJOR 1
#define Editbox_EnableKbdAdjust_VMINOR 0
#define Editbox_EnableKbdAdjust_VPATCH 0
#define Editbox_EnableKbdAdjust_VTAIL  1

#define Editbox_EnableKbdAdjuststr__(n) #n
#define Editbox_EnableKbdAdjuststr(n) Editbox_EnableKbdAdjuststr__(n)

// The following 4 are used in .rc
#define Editbox_EnableKbdAdjust_VMAJORs Editbox_EnableKbdAdjuststr(Editbox_EnableKbdAdjust_VMAJOR)
#define Editbox_EnableKbdAdjust_VMINORs Editbox_EnableKbdAdjuststr(Editbox_EnableKbdAdjust_VMINOR)
#define Editbox_EnableKbdAdjust_VPATCHs Editbox_EnableKbdAdjuststr(Editbox_EnableKbdAdjust_VPATCH)
#define Editbox_EnableKbdAdjust_VTAILs  Editbox_EnableKbdAdjuststr(Editbox_EnableKbdAdjust_VTAIL)

#define Editbox_EnableKbdAdjust_NAME "Editbox_EnableKbdAdjust"

enum {
	Editbox_EnableKbdAdjust_vmajor = Editbox_EnableKbdAdjust_VMAJOR,
	Editbox_EnableKbdAdjust_vminor = Editbox_EnableKbdAdjust_VMINOR,
	Editbox_EnableKbdAdjust_vpatch = Editbox_EnableKbdAdjust_VPATCH,
	Editbox_EnableKbdAdjust_vtail = Editbox_EnableKbdAdjust_VTAIL,
};


#endif
