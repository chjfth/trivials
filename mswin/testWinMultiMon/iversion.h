#ifndef __testWinMultiMon_iversion_h_
#define __testWinMultiMon_iversion_h_

#define testWinMultiMon_VMAJOR 1
#define testWinMultiMon_VMINOR 0
#define testWinMultiMon_VPATCH 0
#define testWinMultiMon_VTAIL  1

#define testWinMultiMonstr__(n) #n
#define testWinMultiMonstr(n) testWinMultiMonstr__(n)

// The following 4 are used in .rc
#define testWinMultiMon_VMAJORs testWinMultiMonstr(testWinMultiMon_VMAJOR)
#define testWinMultiMon_VMINORs testWinMultiMonstr(testWinMultiMon_VMINOR)
#define testWinMultiMon_VPATCHs testWinMultiMonstr(testWinMultiMon_VPATCH)
#define testWinMultiMon_VTAILs  testWinMultiMonstr(testWinMultiMon_VTAIL)

#define testWinMultiMon_NAME "testWinMultiMon"

enum {
	testWinMultiMon_vmajor = testWinMultiMon_VMAJOR,
	testWinMultiMon_vminor = testWinMultiMon_VMINOR,
	testWinMultiMon_vpatch = testWinMultiMon_VPATCH,
	testWinMultiMon_vtail = testWinMultiMon_VTAIL,
};


#endif
