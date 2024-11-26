#ifndef __cpuhog_gui_iversion_h_
#define __cpuhog_gui_iversion_h_

#define cpuhog_gui_VMAJOR 1
#define cpuhog_gui_VMINOR 0
#define cpuhog_gui_VPATCH 0
#define cpuhog_gui_VTAIL  1

#define cpuhog_guistr__(n) #n
#define cpuhog_guistr(n) cpuhog_guistr__(n)

// The following 4 are used in .rc
#define cpuhog_gui_VMAJORs cpuhog_guistr(cpuhog_gui_VMAJOR)
#define cpuhog_gui_VMINORs cpuhog_guistr(cpuhog_gui_VMINOR)
#define cpuhog_gui_VPATCHs cpuhog_guistr(cpuhog_gui_VPATCH)
#define cpuhog_gui_VTAILs  cpuhog_guistr(cpuhog_gui_VTAIL)

#define cpuhog_gui_NAME "cpuhog_gui"

enum {
	cpuhog_gui_vmajor = cpuhog_gui_VMAJOR,
	cpuhog_gui_vminor = cpuhog_gui_VMINOR,
	cpuhog_gui_vpatch = cpuhog_gui_VPATCH,
	cpuhog_gui_vtail = cpuhog_gui_VTAIL,
};


#endif
