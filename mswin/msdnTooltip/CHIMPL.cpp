#include "shareinc.h"

#include <utility>       // to have std::forward
#include <CxxVerCheck.h> // to see CXX11_OR_NEWER

#include "../utils.h"

#ifdef CXX11_OR_NEWER
template<typename... Args>
void vaDBG(Args&&... args) // forwards all arguments
{
	vaDbgTs(std::forward<Args>(args)...);
}
#else
void vaDBG(...)
{
	vaDbgTs(_T("Not C++11 compiled, vaDBG() is nullop."));
};
#endif


#define JULAYOUT_IMPL
#include <mswin/JULayout2.h>

#define   ModelessChild_DEBUG
#define   ModelessChild_IMPL
#include "ModelessChild.h"

#define   TtDlgForUic_DEBUG
#define   TtDlgForUic_IMPL
#include "TtDlgForUic.h"

#define   TtDlgForRectArea_DEBUG
#define   TtDlgForRectArea_IMPL
#include "TtDlgForRectArea.h"

#define   TtDlgTrackingToolTip_DEBUG
#define   TtDlgTrackingToolTip_IMPL
#include "TtDlgTrackingToolTip.h"

#define   TtDlgInplaceSimplest_DEBUG
#define   TtDlgInplaceSimplest_IMPL
#include "TtDlgInplaceSimplest.h"

#define   TtDlgInplaceTooltip_DEBUG
#define   TtDlgInplaceTooltip_IMPL
#include "TtDlgInplaceTooltip.h"

#define   TtDlgMultiline_DEBUG
#define   TtDlgMultiline_IMPL
#include "TtDlgMultiline.h"

