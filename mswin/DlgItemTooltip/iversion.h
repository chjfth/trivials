#ifndef __DlgItemTooltip_iversion_h_
#define __DlgItemTooltip_iversion_h_

#define DlgItemTooltip_VMAJOR 1
#define DlgItemTooltip_VMINOR 0
#define DlgItemTooltip_VPATCH 0
#define DlgItemTooltip_VTAIL  1

#define DlgItemTooltipstr__(n) #n
#define DlgItemTooltipstr(n) DlgItemTooltipstr__(n)

// The following 4 are used in .rc
#define DlgItemTooltip_VMAJORs DlgItemTooltipstr(DlgItemTooltip_VMAJOR)
#define DlgItemTooltip_VMINORs DlgItemTooltipstr(DlgItemTooltip_VMINOR)
#define DlgItemTooltip_VPATCHs DlgItemTooltipstr(DlgItemTooltip_VPATCH)
#define DlgItemTooltip_VTAILs  DlgItemTooltipstr(DlgItemTooltip_VTAIL)

#define DlgItemTooltip_NAME "DlgItemTooltip"

enum {
	DlgItemTooltip_vmajor = DlgItemTooltip_VMAJOR,
	DlgItemTooltip_vminor = DlgItemTooltip_VMINOR,
	DlgItemTooltip_vpatch = DlgItemTooltip_VPATCH,
	DlgItemTooltip_vtail = DlgItemTooltip_VTAIL,
};


#endif
