#ifndef __tPlaySound_iversion_h_
#define __tPlaySound_iversion_h_

#define tPlaySound_VMAJOR 1
#define tPlaySound_VMINOR 2
#define tPlaySound_VPATCH 1
#define tPlaySound_VTAIL  0

#define tPlaySoundstr__(n) #n
#define tPlaySoundstr(n) tPlaySoundstr__(n)

// The following 4 are used in .rc
#define tPlaySound_VMAJORs tPlaySoundstr(tPlaySound_VMAJOR)
#define tPlaySound_VMINORs tPlaySoundstr(tPlaySound_VMINOR)
#define tPlaySound_VPATCHs tPlaySoundstr(tPlaySound_VPATCH)
#define tPlaySound_VTAILs  tPlaySoundstr(tPlaySound_VTAIL)

#define tPlaySound_NAME "tPlaySound"

enum {
	tPlaySound_vmajor = tPlaySound_VMAJOR,
	tPlaySound_vminor = tPlaySound_VMINOR,
	tPlaySound_vpatch = tPlaySound_VPATCH,
	tPlaySound_vtail = tPlaySound_VTAIL,
};


#endif
