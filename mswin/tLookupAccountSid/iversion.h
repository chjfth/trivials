#ifndef __tLookupAccountSid_iversion_h_
#define __tLookupAccountSid_iversion_h_

#define tLookupAccountSid_VMAJOR 1
#define tLookupAccountSid_VMINOR 0
#define tLookupAccountSid_VPATCH 3
#define tLookupAccountSid_VTAIL  0

#define tLookupAccountSidstr__(n) #n
#define tLookupAccountSidstr(n) tLookupAccountSidstr__(n)

// The following 4 are used in .rc
#define tLookupAccountSid_VMAJORs tLookupAccountSidstr(tLookupAccountSid_VMAJOR)
#define tLookupAccountSid_VMINORs tLookupAccountSidstr(tLookupAccountSid_VMINOR)
#define tLookupAccountSid_VPATCHs tLookupAccountSidstr(tLookupAccountSid_VPATCH)
#define tLookupAccountSid_VTAILs  tLookupAccountSidstr(tLookupAccountSid_VTAIL)

#define tLookupAccountSid_NAME "tLookupAccountSid"

enum {
	tLookupAccountSid_vmajor = tLookupAccountSid_VMAJOR,
	tLookupAccountSid_vminor = tLookupAccountSid_VMINOR,
	tLookupAccountSid_vpatch = tLookupAccountSid_VPATCH,
	tLookupAccountSid_vtail = tLookupAccountSid_VTAIL,
};


#endif
