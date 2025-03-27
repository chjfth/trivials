#ifndef __SimpleCreateFile_iversion_h_
#define __SimpleCreateFile_iversion_h_

#define SimpleCreateFile_VMAJOR 1
#define SimpleCreateFile_VMINOR 0
#define SimpleCreateFile_VPATCH 5
#define SimpleCreateFile_VTAIL  0

#define SimpleCreateFilestr__(n) #n
#define SimpleCreateFilestr(n) SimpleCreateFilestr__(n)

// The following 4 are used in .rc
#define SimpleCreateFile_VMAJORs SimpleCreateFilestr(SimpleCreateFile_VMAJOR)
#define SimpleCreateFile_VMINORs SimpleCreateFilestr(SimpleCreateFile_VMINOR)
#define SimpleCreateFile_VPATCHs SimpleCreateFilestr(SimpleCreateFile_VPATCH)
#define SimpleCreateFile_VTAILs  SimpleCreateFilestr(SimpleCreateFile_VTAIL)

#define SimpleCreateFile_NAME "SimpleCreateFile"

enum {
	SimpleCreateFile_vmajor = SimpleCreateFile_VMAJOR,
	SimpleCreateFile_vminor = SimpleCreateFile_VMINOR,
	SimpleCreateFile_vpatch = SimpleCreateFile_VPATCH,
	SimpleCreateFile_vtail = SimpleCreateFile_VTAIL,
};


#endif
