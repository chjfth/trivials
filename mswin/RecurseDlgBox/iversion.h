#ifndef __RecurseDlgBox_iversion_h_
#define __RecurseDlgBox_iversion_h_

#define RecurseDlgBox_VMAJOR 1
#define RecurseDlgBox_VMINOR 0
#define RecurseDlgBox_VPATCH 0
#define RecurseDlgBox_VTAIL  1

#define RecurseDlgBoxstr__(n) #n
#define RecurseDlgBoxstr(n) RecurseDlgBoxstr__(n)

// The following 4 are used in .rc
#define RecurseDlgBox_VMAJORs RecurseDlgBoxstr(RecurseDlgBox_VMAJOR)
#define RecurseDlgBox_VMINORs RecurseDlgBoxstr(RecurseDlgBox_VMINOR)
#define RecurseDlgBox_VPATCHs RecurseDlgBoxstr(RecurseDlgBox_VPATCH)
#define RecurseDlgBox_VTAILs  RecurseDlgBoxstr(RecurseDlgBox_VTAIL)

#define RecurseDlgBox_NAME "RecurseDlgBox"

enum {
	RecurseDlgBox_vmajor = RecurseDlgBox_VMAJOR,
	RecurseDlgBox_vminor = RecurseDlgBox_VMINOR,
	RecurseDlgBox_vpatch = RecurseDlgBox_VPATCH,
	RecurseDlgBox_vtail = RecurseDlgBox_VTAIL,
};


#endif
