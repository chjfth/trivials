#ifndef __CreateProcessAsUser1_iversion_h_
#define __CreateProcessAsUser1_iversion_h_

#define CreateProcessAsUser1_VMAJOR 1
#define CreateProcessAsUser1_VMINOR 0
#define CreateProcessAsUser1_VPATCH 2
#define CreateProcessAsUser1_VTAIL  0

#define CreateProcessAsUser1str__(n) #n
#define CreateProcessAsUser1str(n) CreateProcessAsUser1str__(n)

// The following 4 are used in .rc
#define CreateProcessAsUser1_VMAJORs CreateProcessAsUser1str(CreateProcessAsUser1_VMAJOR)
#define CreateProcessAsUser1_VMINORs CreateProcessAsUser1str(CreateProcessAsUser1_VMINOR)
#define CreateProcessAsUser1_VPATCHs CreateProcessAsUser1str(CreateProcessAsUser1_VPATCH)
#define CreateProcessAsUser1_VTAILs  CreateProcessAsUser1str(CreateProcessAsUser1_VTAIL)

#define CreateProcessAsUser1_NAME "CreateProcessAsUser1"

enum {
	CreateProcessAsUser1_vmajor = CreateProcessAsUser1_VMAJOR,
	CreateProcessAsUser1_vminor = CreateProcessAsUser1_VMINOR,
	CreateProcessAsUser1_vpatch = CreateProcessAsUser1_VPATCH,
	CreateProcessAsUser1_vtail = CreateProcessAsUser1_VTAIL,
};


#endif
