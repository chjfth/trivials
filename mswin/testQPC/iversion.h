#ifndef __testQPC_iversion_h_
#define __testQPC_iversion_h_

#define testQPC_VMAJOR 1
#define testQPC_VMINOR 0
#define testQPC_VPATCH 1
#define testQPC_VTAIL  0

#define testQPCstr__(n) #n
#define testQPCstr(n) testQPCstr__(n)

// The following 4 are used in .rc
#define testQPC_VMAJORs testQPCstr(testQPC_VMAJOR)
#define testQPC_VMINORs testQPCstr(testQPC_VMINOR)
#define testQPC_VPATCHs testQPCstr(testQPC_VPATCH)
#define testQPC_VTAILs  testQPCstr(testQPC_VTAIL)

#define testQPC_NAME "testQPC"

enum {
	testQPC_vmajor = testQPC_VMAJOR,
	testQPC_vminor = testQPC_VMINOR,
	testQPC_vpatch = testQPC_VPATCH,
	testQPC_vtail = testQPC_VTAIL,
};


#endif
