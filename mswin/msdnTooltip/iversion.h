#ifndef __msdnTooltip_iversion_h_
#define __msdnTooltip_iversion_h_

#define msdnTooltip_VMAJOR 1
#define msdnTooltip_VMINOR 3
#define msdnTooltip_VPATCH 3
#define msdnTooltip_VTAIL  0

#define msdnTooltipstr__(n) #n
#define msdnTooltipstr(n) msdnTooltipstr__(n)

// The following 4 are used in .rc
#define msdnTooltip_VMAJORs msdnTooltipstr(msdnTooltip_VMAJOR)
#define msdnTooltip_VMINORs msdnTooltipstr(msdnTooltip_VMINOR)
#define msdnTooltip_VPATCHs msdnTooltipstr(msdnTooltip_VPATCH)
#define msdnTooltip_VTAILs  msdnTooltipstr(msdnTooltip_VTAIL)

#define msdnTooltip_NAME "msdnTooltip"

enum {
	msdnTooltip_vmajor = msdnTooltip_VMAJOR,
	msdnTooltip_vminor = msdnTooltip_VMINOR,
	msdnTooltip_vpatch = msdnTooltip_VPATCH,
	msdnTooltip_vtail = msdnTooltip_VTAIL,
};


#endif
