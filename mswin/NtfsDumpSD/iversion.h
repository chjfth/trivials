#ifndef __NtfsDumpSD_iversion_h_
#define __NtfsDumpSD_iversion_h_

#define NtfsDumpSD_VMAJOR 1
#define NtfsDumpSD_VMINOR 3
#define NtfsDumpSD_VPATCH 3
#define NtfsDumpSD_VTAIL  0

#define NtfsDumpSDstr__(n) #n
#define NtfsDumpSDstr(n) NtfsDumpSDstr__(n)

// The following 4 are used in .rc
#define NtfsDumpSD_VMAJORs NtfsDumpSDstr(NtfsDumpSD_VMAJOR)
#define NtfsDumpSD_VMINORs NtfsDumpSDstr(NtfsDumpSD_VMINOR)
#define NtfsDumpSD_VPATCHs NtfsDumpSDstr(NtfsDumpSD_VPATCH)
#define NtfsDumpSD_VTAILs  NtfsDumpSDstr(NtfsDumpSD_VTAIL)

#define NtfsDumpSD_NAME "NtfsDumpSD"

enum {
	NtfsDumpSD_vmajor = NtfsDumpSD_VMAJOR,
	NtfsDumpSD_vminor = NtfsDumpSD_VMINOR,
	NtfsDumpSD_vpatch = NtfsDumpSD_VPATCH,
	NtfsDumpSD_vtail = NtfsDumpSD_VTAIL,
};


#endif
