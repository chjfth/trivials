#ifndef __DlgBaseUnits_iversion_h_
#define __DlgBaseUnits_iversion_h_

#define DlgBaseUnits_VMAJOR 1
#define DlgBaseUnits_VMINOR 0
#define DlgBaseUnits_VPATCH 0
#define DlgBaseUnits_VTAIL  1

#define DlgBaseUnitsstr__(n) #n
#define DlgBaseUnitsstr(n) DlgBaseUnitsstr__(n)

// The following 4 are used in .rc
#define DlgBaseUnits_VMAJORs DlgBaseUnitsstr(DlgBaseUnits_VMAJOR)
#define DlgBaseUnits_VMINORs DlgBaseUnitsstr(DlgBaseUnits_VMINOR)
#define DlgBaseUnits_VPATCHs DlgBaseUnitsstr(DlgBaseUnits_VPATCH)
#define DlgBaseUnits_VTAILs  DlgBaseUnitsstr(DlgBaseUnits_VTAIL)

#define DlgBaseUnits_NAME "DlgBaseUnits"

enum {
	DlgBaseUnits_vmajor = DlgBaseUnits_VMAJOR,
	DlgBaseUnits_vminor = DlgBaseUnits_VMINOR,
	DlgBaseUnits_vpatch = DlgBaseUnits_VPATCH,
	DlgBaseUnits_vtail = DlgBaseUnits_VTAIL,
};


#endif
