#ifndef __CreateProcessOnto_iversion_h_
#define __CreateProcessOnto_iversion_h_

#define CreateProcessOnto_VMAJOR 1
#define CreateProcessOnto_VMINOR 0
#define CreateProcessOnto_VPATCH 0
#define CreateProcessOnto_VTAIL  1

#define CreateProcessOntostr__(n) #n
#define CreateProcessOntostr(n) CreateProcessOntostr__(n)

// The following 4 are used in .rc
#define CreateProcessOnto_VMAJORs CreateProcessOntostr(CreateProcessOnto_VMAJOR)
#define CreateProcessOnto_VMINORs CreateProcessOntostr(CreateProcessOnto_VMINOR)
#define CreateProcessOnto_VPATCHs CreateProcessOntostr(CreateProcessOnto_VPATCH)
#define CreateProcessOnto_VTAILs  CreateProcessOntostr(CreateProcessOnto_VTAIL)

#define CreateProcessOnto_NAME "CreateProcessOnto"

enum {
	CreateProcessOnto_vmajor = CreateProcessOnto_VMAJOR,
	CreateProcessOnto_vminor = CreateProcessOnto_VMINOR,
	CreateProcessOnto_vpatch = CreateProcessOnto_VPATCH,
	CreateProcessOnto_vtail = CreateProcessOnto_VTAIL,
};


#endif
