#ifndef __testWindowFromPoint_iversion_h_
#define __testWindowFromPoint_iversion_h_

#define testWindowFromPoint_VMAJOR 1
#define testWindowFromPoint_VMINOR 1
#define testWindowFromPoint_VPATCH 0
#define testWindowFromPoint_VTAIL  0

#define testWindowFromPointstr__(n) #n
#define testWindowFromPointstr(n) testWindowFromPointstr__(n)

// The following 4 are used in .rc
#define testWindowFromPoint_VMAJORs testWindowFromPointstr(testWindowFromPoint_VMAJOR)
#define testWindowFromPoint_VMINORs testWindowFromPointstr(testWindowFromPoint_VMINOR)
#define testWindowFromPoint_VPATCHs testWindowFromPointstr(testWindowFromPoint_VPATCH)
#define testWindowFromPoint_VTAILs  testWindowFromPointstr(testWindowFromPoint_VTAIL)

#define testWindowFromPoint_NAME "testWindowFromPoint"

enum {
	testWindowFromPoint_vmajor = testWindowFromPoint_VMAJOR,
	testWindowFromPoint_vminor = testWindowFromPoint_VMINOR,
	testWindowFromPoint_vpatch = testWindowFromPoint_VPATCH,
	testWindowFromPoint_vtail = testWindowFromPoint_VTAIL,
};


#endif
