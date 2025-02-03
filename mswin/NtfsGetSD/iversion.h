#ifndef __NtfsGetSD_iversion_h_
#define __NtfsGetSD_iversion_h_

#define NtfsGetSD_VMAJOR 1
#define NtfsGetSD_VMINOR 0
#define NtfsGetSD_VPATCH 0
#define NtfsGetSD_VTAIL  1

#define NtfsGetSDstr__(n) #n
#define NtfsGetSDstr(n) NtfsGetSDstr__(n)

// The following 4 are used in .rc
#define NtfsGetSD_VMAJORs NtfsGetSDstr(NtfsGetSD_VMAJOR)
#define NtfsGetSD_VMINORs NtfsGetSDstr(NtfsGetSD_VMINOR)
#define NtfsGetSD_VPATCHs NtfsGetSDstr(NtfsGetSD_VPATCH)
#define NtfsGetSD_VTAILs  NtfsGetSDstr(NtfsGetSD_VTAIL)

#define NtfsGetSD_NAME "NtfsGetSD"

enum {
	NtfsGetSD_vmajor = NtfsGetSD_VMAJOR,
	NtfsGetSD_vminor = NtfsGetSD_VMINOR,
	NtfsGetSD_vpatch = NtfsGetSD_VPATCH,
	NtfsGetSD_vtail = NtfsGetSD_VTAIL,
};


#endif
