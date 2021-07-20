/******************************************************************************
 LLDBGetBreakpoints.cpp

	BASE CLASS = CMGetBreakpoints

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LLDBGetBreakpoints.h"
#include "CMBreakpointManager.h"
#include "LLDBLink.h"
#include "lldb/API/SBBreakpoint.h"
#include "lldb/API/SBBreakpointLocation.h"
#include "lldb/API/SBAddress.h"
#include "lldb/API/SBFunction.h"
#include "lldb/API/SBLineEntry.h"
#include "lldb/API/SBFileSpec.h"
#include "cmGlobals.h"
#include <jFileUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LLDBGetBreakpoints::LLDBGetBreakpoints()
	:
	CMGetBreakpoints(JString::empty)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LLDBGetBreakpoints::~LLDBGetBreakpoints()
{
}

/******************************************************************************
 HandleSuccess (virtual protected)

 *****************************************************************************/

void
LLDBGetBreakpoints::HandleSuccess
	(
	const JString& cmdData
	)
{
	auto* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == nullptr)
		{
		return;
		}

	lldb::SBTarget t = link->GetDebugger()->GetSelectedTarget();
	if (!t.IsValid())
		{
		return;
		}

	(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(false);

	JPtrArray<CMBreakpoint> bpList(JPtrArrayT::kForgetAll);	// ownership taken by CMBreakpointManager
	bpList.SetCompareFunction(CMBreakpointManager::CompareBreakpointLocations);
	bpList.SetSortOrder(JListT::kSortAscending);

	JPtrArray<CMBreakpoint> otherList(JPtrArrayT::kForgetAll);	// ownership taken by CMBreakpointManager

	JSize count = t.GetNumBreakpoints();
	JString func, addr, cond;
	for (JUnsignedOffset i=0; i<count; i++)
		{
		lldb::SBBreakpoint b = t.GetBreakpointAtIndex(i);
		if (!b.IsValid() || b.GetNumLocations() == 0)
			{
			continue;
			}

		lldb::SBBreakpointLocation loc = b.GetLocationAtIndex(0);
		lldb::SBAddress a              = loc.GetAddress();
		lldb::SBFunction fn            = a.GetFunction();
		lldb::SBLineEntry e            = a.GetLineEntry();
		lldb::SBFileSpec file          = e.GetFileSpec();

		const CMBreakpoint::Action action = b.IsOneShot() ?
			CMBreakpoint::kRemoveBreakpoint :
			CMBreakpoint::kKeepBreakpoint;

		if (fn.IsValid())
			{
			func = fn.GetName();
			addr = JString(a.GetLoadAddress(t), JString::kBase16);
			}
		else
			{
			func.Clear();
			addr.Clear();
			}

		if (b.GetCondition() != nullptr)
			{
			cond = b.GetCondition();
			}
		else
			{
			cond.Clear();
			}

		CMBreakpoint* bp = nullptr;
		if (file.IsValid())
			{
			const JString fullName = JCombinePathAndName(
				JString(file.GetDirectory(), JString::kNoCopy),
				JString(file.GetFilename(), JString::kNoCopy));

			bp = jnew CMBreakpoint(b.GetID(), fullName, e.GetLine(), func, addr,
								  b.IsEnabled(), action,
								  cond, b.GetIgnoreCount());
			assert( bp != nullptr );
			}
		else if (fn.IsValid())
			{
			CMLocation loc;
			bp = jnew CMBreakpoint(b.GetID(), loc, func, addr,
								  b.IsEnabled(), action,
								  cond, b.GetIgnoreCount());
			assert( bp != nullptr );
			}

		if (bp != nullptr)
			{
			bpList.InsertSorted(bp);

			// may be deleted or other status change

			if (action != CMBreakpoint::kKeepBreakpoint || b.GetIgnoreCount() > 0)
				{
				(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(true);
				}
			}
		}

	count = t.GetNumWatchpoints();
	if (count > 0)	// may be deleted when go out of scope
		{
		(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(true);
		}

	for (JUnsignedOffset i=0; i<count; i++)
		{
		lldb::SBWatchpoint w = t.GetWatchpointAtIndex(i);

		CMLocation loc;
		addr = JString(w.GetWatchAddress(), JString::kBase16);

		if (w.GetCondition() != nullptr)
			{
			cond = w.GetCondition();
			}
		else
			{
			cond.Clear();
			}

		auto* bp = jnew CMBreakpoint(w.GetID(), loc, JString::empty, addr,
											 w.IsEnabled(), CMBreakpoint::kKeepBreakpoint,
											 cond, w.GetIgnoreCount());
		assert( bp != nullptr );
		bpList.InsertSorted(bp);
		}

	CMGetLink()->GetBreakpointManager()->UpdateBreakpoints(bpList, otherList);
}
