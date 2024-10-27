#ifndef __WM_TIMER_accuracy_iversion_h_
#define __WM_TIMER_accuracy_iversion_h_

#define WM_TIMER_accuracy_VMAJOR 1
#define WM_TIMER_accuracy_VMINOR 3
#define WM_TIMER_accuracy_VPATCH 1
#define WM_TIMER_accuracy_VTAIL  0

#define WM_TIMER_accuracystr__(n) #n
#define WM_TIMER_accuracystr(n) WM_TIMER_accuracystr__(n)

// The following 4 are used in .rc
#define WM_TIMER_accuracy_VMAJORs WM_TIMER_accuracystr(WM_TIMER_accuracy_VMAJOR)
#define WM_TIMER_accuracy_VMINORs WM_TIMER_accuracystr(WM_TIMER_accuracy_VMINOR)
#define WM_TIMER_accuracy_VPATCHs WM_TIMER_accuracystr(WM_TIMER_accuracy_VPATCH)
#define WM_TIMER_accuracy_VTAILs  WM_TIMER_accuracystr(WM_TIMER_accuracy_VTAIL)

#define WM_TIMER_accuracy_NAME "WM_TIMER_accuracy"

enum {
	WM_TIMER_accuracy_vmajor = WM_TIMER_accuracy_VMAJOR,
	WM_TIMER_accuracy_vminor = WM_TIMER_accuracy_VMINOR,
	WM_TIMER_accuracy_vpatch = WM_TIMER_accuracy_VPATCH,
	WM_TIMER_accuracy_vtail = WM_TIMER_accuracy_VTAIL,
};


#endif
