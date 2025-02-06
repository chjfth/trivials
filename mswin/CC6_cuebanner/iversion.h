#ifndef __CC6_cuebanner_iversion_h_
#define __CC6_cuebanner_iversion_h_

#define CC6_cuebanner_VMAJOR 1
#define CC6_cuebanner_VMINOR 0
#define CC6_cuebanner_VPATCH 0
#define CC6_cuebanner_VTAIL  1

#define CC6_cuebannerstr__(n) #n
#define CC6_cuebannerstr(n) CC6_cuebannerstr__(n)

// The following 4 are used in .rc
#define CC6_cuebanner_VMAJORs CC6_cuebannerstr(CC6_cuebanner_VMAJOR)
#define CC6_cuebanner_VMINORs CC6_cuebannerstr(CC6_cuebanner_VMINOR)
#define CC6_cuebanner_VPATCHs CC6_cuebannerstr(CC6_cuebanner_VPATCH)
#define CC6_cuebanner_VTAILs  CC6_cuebannerstr(CC6_cuebanner_VTAIL)

#define CC6_cuebanner_NAME "CC6_cuebanner"

enum {
	CC6_cuebanner_vmajor = CC6_cuebanner_VMAJOR,
	CC6_cuebanner_vminor = CC6_cuebanner_VMINOR,
	CC6_cuebanner_vpatch = CC6_cuebanner_VPATCH,
	CC6_cuebanner_vtail = CC6_cuebanner_VTAIL,
};


#endif
