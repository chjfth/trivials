#ifndef __DlgEscNoClose1_iversion_h_
#define __DlgEscNoClose1_iversion_h_

#define DlgEscNoClose1_VMAJOR 1
#define DlgEscNoClose1_VMINOR 0
#define DlgEscNoClose1_VPATCH 0
#define DlgEscNoClose1_VTAIL  1

#define DlgEscNoClose1str__(n) #n
#define DlgEscNoClose1str(n) DlgEscNoClose1str__(n)

// The following 4 are used in .rc
#define DlgEscNoClose1_VMAJORs DlgEscNoClose1str(DlgEscNoClose1_VMAJOR)
#define DlgEscNoClose1_VMINORs DlgEscNoClose1str(DlgEscNoClose1_VMINOR)
#define DlgEscNoClose1_VPATCHs DlgEscNoClose1str(DlgEscNoClose1_VPATCH)
#define DlgEscNoClose1_VTAILs  DlgEscNoClose1str(DlgEscNoClose1_VTAIL)

#define DlgEscNoClose1_NAME "DlgEscNoClose1"

enum {
	DlgEscNoClose1_vmajor = DlgEscNoClose1_VMAJOR,
	DlgEscNoClose1_vminor = DlgEscNoClose1_VMINOR,
	DlgEscNoClose1_vpatch = DlgEscNoClose1_VPATCH,
	DlgEscNoClose1_vtail = DlgEscNoClose1_VTAIL,
};


#endif
