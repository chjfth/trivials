#ifndef __KbdLayoutList_iversion_h_
#define __KbdLayoutList_iversion_h_


#define KbdLayoutList_VMAJOR 1
#define KbdLayoutList_VMINOR 0
#define KbdLayoutList_VPATCH 0
#define KbdLayoutList_VTAIL  1

#define KbdLayoutListstr__(n) #n
#define KbdLayoutListstr(n) KbdLayoutListstr__(n)

// The following 4 are used in .rc
#define KbdLayoutList_VMAJORs KbdLayoutListstr(KbdLayoutList_VMAJOR)
#define KbdLayoutList_VMINORs KbdLayoutListstr(KbdLayoutList_VMINOR)
#define KbdLayoutList_VPATCHs KbdLayoutListstr(KbdLayoutList_VPATCH)
#define KbdLayoutList_VTAILs  KbdLayoutListstr(KbdLayoutList_VTAIL)

#define KbdLayoutList_NAME "KbdLayoutList"

enum {
	KbdLayoutList_vmajor = KbdLayoutList_VMAJOR,
	KbdLayoutList_vminor = KbdLayoutList_VMINOR,
	KbdLayoutList_vpatch = KbdLayoutList_VPATCH,
	KbdLayoutList_vtail = KbdLayoutList_VTAIL,
};


#endif
