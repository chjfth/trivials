#ifndef __DlgItemTooltip2_iversion_h_
#define __DlgItemTooltip2_iversion_h_

#define DlgItemTooltip2_VMAJOR 2
#define DlgItemTooltip2_VMINOR 0
#define DlgItemTooltip2_VPATCH 0
#define DlgItemTooltip2_VTAIL  0

#define DlgItemTooltip2str__(n) #n
#define DlgItemTooltip2str(n) DlgItemTooltip2str__(n)

// The following 4 are used in .rc
#define DlgItemTooltip2_VMAJORs DlgItemTooltip2str(DlgItemTooltip2_VMAJOR)
#define DlgItemTooltip2_VMINORs DlgItemTooltip2str(DlgItemTooltip2_VMINOR)
#define DlgItemTooltip2_VPATCHs DlgItemTooltip2str(DlgItemTooltip2_VPATCH)
#define DlgItemTooltip2_VTAILs  DlgItemTooltip2str(DlgItemTooltip2_VTAIL)

#define DlgItemTooltip2_NAME "DlgItemTooltip2"

enum {
	DlgItemTooltip2_vmajor = DlgItemTooltip2_VMAJOR,
	DlgItemTooltip2_vminor = DlgItemTooltip2_VMINOR,
	DlgItemTooltip2_vpatch = DlgItemTooltip2_VPATCH,
	DlgItemTooltip2_vtail = DlgItemTooltip2_VTAIL,
};


#endif
