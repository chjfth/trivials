#ifndef __test_SS_NOTIFY_iversion_h_
#define __test_SS_NOTIFY_iversion_h_

#define test_SS_NOTIFY_VMAJOR 1
#define test_SS_NOTIFY_VMINOR 0
#define test_SS_NOTIFY_VPATCH 0
#define test_SS_NOTIFY_VTAIL  1

#define test_SS_NOTIFYstr__(n) #n
#define test_SS_NOTIFYstr(n) test_SS_NOTIFYstr__(n)

// The following 4 are used in .rc
#define test_SS_NOTIFY_VMAJORs test_SS_NOTIFYstr(test_SS_NOTIFY_VMAJOR)
#define test_SS_NOTIFY_VMINORs test_SS_NOTIFYstr(test_SS_NOTIFY_VMINOR)
#define test_SS_NOTIFY_VPATCHs test_SS_NOTIFYstr(test_SS_NOTIFY_VPATCH)
#define test_SS_NOTIFY_VTAILs  test_SS_NOTIFYstr(test_SS_NOTIFY_VTAIL)

#define test_SS_NOTIFY_NAME "test_SS_NOTIFY"

enum {
	test_SS_NOTIFY_vmajor = test_SS_NOTIFY_VMAJOR,
	test_SS_NOTIFY_vminor = test_SS_NOTIFY_VMINOR,
	test_SS_NOTIFY_vpatch = test_SS_NOTIFY_VPATCH,
	test_SS_NOTIFY_vtail = test_SS_NOTIFY_VTAIL,
};


#endif
