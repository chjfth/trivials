#ifndef __MyLib_h_
#define __MyLib_h_

#ifdef __cplusplus
extern"C" {
#endif


struct MyLib_version_st 
{
	int major, minor, patch, extra;
};


bool MyLib_getversion(struct MyLib_version_st *pver);


void MyLib_printversion();



#ifdef __cplusplus
} // extern"C" {
#endif

#endif
