#ifndef __guiSetTimeZone_iversion_h_
#define __guiSetTimeZone_iversion_h_

#define guiSetTimeZone_VMAJOR 1
#define guiSetTimeZone_VMINOR 0
#define guiSetTimeZone_VPATCH 1
#define guiSetTimeZone_VTAIL  0

#define guiSetTimeZonestr__(n) #n
#define guiSetTimeZonestr(n) guiSetTimeZonestr__(n)

// The following 4 are used in .rc
#define guiSetTimeZone_VMAJORs guiSetTimeZonestr(guiSetTimeZone_VMAJOR)
#define guiSetTimeZone_VMINORs guiSetTimeZonestr(guiSetTimeZone_VMINOR)
#define guiSetTimeZone_VPATCHs guiSetTimeZonestr(guiSetTimeZone_VPATCH)
#define guiSetTimeZone_VTAILs  guiSetTimeZonestr(guiSetTimeZone_VTAIL)

#define guiSetTimeZone_NAME "guiSetTimeZone"

enum {
	guiSetTimeZone_vmajor = guiSetTimeZone_VMAJOR,
	guiSetTimeZone_vminor = guiSetTimeZone_VMINOR,
	guiSetTimeZone_vpatch = guiSetTimeZone_VPATCH,
	guiSetTimeZone_vtail = guiSetTimeZone_VTAIL,
};


#endif
