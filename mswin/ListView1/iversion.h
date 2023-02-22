#ifndef __ListView1_iversion_h_
#define __ListView1_iversion_h_


#define ListView1_VMAJOR 1
#define ListView1_VMINOR 0
#define ListView1_VPATCH 0
#define ListView1_VTAIL  1

#define ListView1str__(n) #n
#define ListView1str(n) ListView1str__(n)

// The following 4 are used in .rc
#define ListView1_VMAJORs ListView1str(ListView1_VMAJOR)
#define ListView1_VMINORs ListView1str(ListView1_VMINOR)
#define ListView1_VPATCHs ListView1str(ListView1_VPATCH)
#define ListView1_VTAILs  ListView1str(ListView1_VTAIL)

#define ListView1_NAME "ListView1"

enum {
	ListView1_vmajor = ListView1_VMAJOR,
	ListView1_vminor = ListView1_VMINOR,
	ListView1_vpatch = ListView1_VPATCH,
	ListView1_vtail = ListView1_VTAIL,
};


#endif
