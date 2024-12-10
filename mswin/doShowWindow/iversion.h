#ifndef __doShowWindow_iversion_h_
#define __doShowWindow_iversion_h_

#define doShowWindow_VMAJOR 1
#define doShowWindow_VMINOR 2
#define doShowWindow_VPATCH 0
#define doShowWindow_VTAIL  0

#define doShowWindowstr__(n) #n
#define doShowWindowstr(n) doShowWindowstr__(n)

// The following 4 are used in .rc
#define doShowWindow_VMAJORs doShowWindowstr(doShowWindow_VMAJOR)
#define doShowWindow_VMINORs doShowWindowstr(doShowWindow_VMINOR)
#define doShowWindow_VPATCHs doShowWindowstr(doShowWindow_VPATCH)
#define doShowWindow_VTAILs  doShowWindowstr(doShowWindow_VTAIL)

#define doShowWindow_NAME "doShowWindow"

enum {
	doShowWindow_vmajor = doShowWindow_VMAJOR,
	doShowWindow_vminor = doShowWindow_VMINOR,
	doShowWindow_vpatch = doShowWindow_VPATCH,
	doShowWindow_vtail = doShowWindow_VTAIL,
};


#endif
