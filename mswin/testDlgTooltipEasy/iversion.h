#ifndef __testDlgTooltipEasy_iversion_h_
#define __testDlgTooltipEasy_iversion_h_

#define testDlgTooltipEasy_VMAJOR 1
#define testDlgTooltipEasy_VMINOR 0
#define testDlgTooltipEasy_VPATCH 4
#define testDlgTooltipEasy_VTAIL  0

#define testDlgTooltipEasystr__(n) #n
#define testDlgTooltipEasystr(n) testDlgTooltipEasystr__(n)

// The following 4 are used in .rc
#define testDlgTooltipEasy_VMAJORs testDlgTooltipEasystr(testDlgTooltipEasy_VMAJOR)
#define testDlgTooltipEasy_VMINORs testDlgTooltipEasystr(testDlgTooltipEasy_VMINOR)
#define testDlgTooltipEasy_VPATCHs testDlgTooltipEasystr(testDlgTooltipEasy_VPATCH)
#define testDlgTooltipEasy_VTAILs  testDlgTooltipEasystr(testDlgTooltipEasy_VTAIL)

#define testDlgTooltipEasy_NAME "testDlgTooltipEasy"

enum {
	testDlgTooltipEasy_vmajor = testDlgTooltipEasy_VMAJOR,
	testDlgTooltipEasy_vminor = testDlgTooltipEasy_VMINOR,
	testDlgTooltipEasy_vpatch = testDlgTooltipEasy_VPATCH,
	testDlgTooltipEasy_vtail = testDlgTooltipEasy_VTAIL,
};


#endif
