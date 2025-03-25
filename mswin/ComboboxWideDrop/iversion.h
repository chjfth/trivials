#ifndef __ComboboxWideDrop_iversion_h_
#define __ComboboxWideDrop_iversion_h_

#define ComboboxWideDrop_VMAJOR 1
#define ComboboxWideDrop_VMINOR 0
#define ComboboxWideDrop_VPATCH 2
#define ComboboxWideDrop_VTAIL  1

#define ComboboxWideDropstr__(n) #n
#define ComboboxWideDropstr(n) ComboboxWideDropstr__(n)

// The following 4 are used in .rc
#define ComboboxWideDrop_VMAJORs ComboboxWideDropstr(ComboboxWideDrop_VMAJOR)
#define ComboboxWideDrop_VMINORs ComboboxWideDropstr(ComboboxWideDrop_VMINOR)
#define ComboboxWideDrop_VPATCHs ComboboxWideDropstr(ComboboxWideDrop_VPATCH)
#define ComboboxWideDrop_VTAILs  ComboboxWideDropstr(ComboboxWideDrop_VTAIL)

#define ComboboxWideDrop_NAME "ComboboxWideDrop"

enum {
	ComboboxWideDrop_vmajor = ComboboxWideDrop_VMAJOR,
	ComboboxWideDrop_vminor = ComboboxWideDrop_VMINOR,
	ComboboxWideDrop_vpatch = ComboboxWideDrop_VPATCH,
	ComboboxWideDrop_vtail = ComboboxWideDrop_VTAIL,
};


#endif
