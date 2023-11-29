#ifndef __MonitorMyWinMove_iversion_h_
#define __MonitorMyWinMove_iversion_h_


#define MonitorMyWinMove_VMAJOR 1
#define MonitorMyWinMove_VMINOR 0
#define MonitorMyWinMove_VPATCH 0
#define MonitorMyWinMove_VTAIL  1

#define MonitorMyWinMovestr__(n) #n
#define MonitorMyWinMovestr(n) MonitorMyWinMovestr__(n)

// The following 4 are used in .rc
#define MonitorMyWinMove_VMAJORs MonitorMyWinMovestr(MonitorMyWinMove_VMAJOR)
#define MonitorMyWinMove_VMINORs MonitorMyWinMovestr(MonitorMyWinMove_VMINOR)
#define MonitorMyWinMove_VPATCHs MonitorMyWinMovestr(MonitorMyWinMove_VPATCH)
#define MonitorMyWinMove_VTAILs  MonitorMyWinMovestr(MonitorMyWinMove_VTAIL)

#define MonitorMyWinMove_NAME "MonitorMyWinMove"

enum {
	MonitorMyWinMove_vmajor = MonitorMyWinMove_VMAJOR,
	MonitorMyWinMove_vminor = MonitorMyWinMove_VMINOR,
	MonitorMyWinMove_vpatch = MonitorMyWinMove_VPATCH,
	MonitorMyWinMove_vtail = MonitorMyWinMove_VTAIL,
};


#endif
