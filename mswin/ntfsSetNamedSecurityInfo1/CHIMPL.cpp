#include <CxxVerCheck.h>

#ifdef CXX11_OR_NEWER
template<typename... Args>
void vaDBG(Args&&... args) // forwards all arguments
{
	PrnTs(std::forward<Args>(args)...);
}
#endif


#define JAUTOBUF_IMPL
#include <JAutoBuf.h>

#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

