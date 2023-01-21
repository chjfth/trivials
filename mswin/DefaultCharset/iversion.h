#ifndef __DefaultCharset_iversion_h_
#define __DefaultCharset_iversion_h_


#define DefaultCharset_VMAJOR 1
#define DefaultCharset_VMINOR 0
#define DefaultCharset_VPATCH 0
#define DefaultCharset_VTAIL  1

#define DefaultCharsetstr__(n) #n
#define DefaultCharsetstr(n) DefaultCharsetstr__(n)

// The following 4 are used in .rc
#define DefaultCharset_VMAJORs DefaultCharsetstr(DefaultCharset_VMAJOR)
#define DefaultCharset_VMINORs DefaultCharsetstr(DefaultCharset_VMINOR)
#define DefaultCharset_VPATCHs DefaultCharsetstr(DefaultCharset_VPATCH)
#define DefaultCharset_VTAILs  DefaultCharsetstr(DefaultCharset_VTAIL)

#define DefaultCharset_NAME "DefaultCharset"

enum {
	DefaultCharset_vmajor = DefaultCharset_VMAJOR,
	DefaultCharset_vminor = DefaultCharset_VMINOR,
	DefaultCharset_vpatch = DefaultCharset_VPATCH,
	DefaultCharset_vtail = DefaultCharset_VTAIL,
};


#endif
