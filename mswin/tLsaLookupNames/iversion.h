#ifndef __tLsaLookupNames_iversion_h_
#define __tLsaLookupNames_iversion_h_

#define tLsaLookupNames_VMAJOR 1
#define tLsaLookupNames_VMINOR 1
#define tLsaLookupNames_VPATCH 0
#define tLsaLookupNames_VTAIL  0

#define tLsaLookupNamesstr__(n) #n
#define tLsaLookupNamesstr(n) tLsaLookupNamesstr__(n)

// The following 4 are used in .rc
#define tLsaLookupNames_VMAJORs tLsaLookupNamesstr(tLsaLookupNames_VMAJOR)
#define tLsaLookupNames_VMINORs tLsaLookupNamesstr(tLsaLookupNames_VMINOR)
#define tLsaLookupNames_VPATCHs tLsaLookupNamesstr(tLsaLookupNames_VPATCH)
#define tLsaLookupNames_VTAILs  tLsaLookupNamesstr(tLsaLookupNames_VTAIL)

#define tLsaLookupNames_NAME "tLsaLookupNames"

enum {
	tLsaLookupNames_vmajor = tLsaLookupNames_VMAJOR,
	tLsaLookupNames_vminor = tLsaLookupNames_VMINOR,
	tLsaLookupNames_vpatch = tLsaLookupNames_VPATCH,
	tLsaLookupNames_vtail = tLsaLookupNames_VTAIL,
};


#endif
