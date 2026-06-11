#ifndef __mciPlaySound_iversion_h_
#define __mciPlaySound_iversion_h_

#define mciPlaySound_VMAJOR 1
#define mciPlaySound_VMINOR 0
#define mciPlaySound_VPATCH 0
#define mciPlaySound_VTAIL  1

#define mciPlaySoundstr__(n) #n
#define mciPlaySoundstr(n) mciPlaySoundstr__(n)

// The following 4 are used in .rc
#define mciPlaySound_VMAJORs mciPlaySoundstr(mciPlaySound_VMAJOR)
#define mciPlaySound_VMINORs mciPlaySoundstr(mciPlaySound_VMINOR)
#define mciPlaySound_VPATCHs mciPlaySoundstr(mciPlaySound_VPATCH)
#define mciPlaySound_VTAILs  mciPlaySoundstr(mciPlaySound_VTAIL)

#define mciPlaySound_NAME "mciPlaySound"

enum {
	mciPlaySound_vmajor = mciPlaySound_VMAJOR,
	mciPlaySound_vminor = mciPlaySound_VMINOR,
	mciPlaySound_vpatch = mciPlaySound_VPATCH,
	mciPlaySound_vtail = mciPlaySound_VTAIL,
};


#endif
