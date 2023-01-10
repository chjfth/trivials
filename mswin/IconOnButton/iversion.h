#ifndef __IconOnButton_iversion_h_
#define __IconOnButton_iversion_h_


#define IconOnButton_VMAJOR 1
#define IconOnButton_VMINOR 0
#define IconOnButton_VPATCH 0
#define IconOnButton_VTAIL  1

#define IconOnButtonstr__(n) #n
#define IconOnButtonstr(n) IconOnButtonstr__(n)

// The following 4 are used in .rc
#define IconOnButton_VMAJORs IconOnButtonstr(IconOnButton_VMAJOR)
#define IconOnButton_VMINORs IconOnButtonstr(IconOnButton_VMINOR)
#define IconOnButton_VPATCHs IconOnButtonstr(IconOnButton_VPATCH)
#define IconOnButton_VTAILs  IconOnButtonstr(IconOnButton_VTAIL)

#define IconOnButton_NAME "IconOnButton"

enum {
	IconOnButton_vmajor = IconOnButton_VMAJOR,
	IconOnButton_vminor = IconOnButton_VMINOR,
	IconOnButton_vpatch = IconOnButton_VPATCH,
	IconOnButton_vtail = IconOnButton_VTAIL,
};


#endif
