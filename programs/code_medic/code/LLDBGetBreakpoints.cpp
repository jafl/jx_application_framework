/******************************************************************************
 LLDBGetBreakpoints.cpp

	BASE CLASS = CMGetBreakpoints

	Copyright (C) 2016 by John Lindal. All rights reserved.

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
	CMGetBreakpoints("")
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
	LLDBLink* link = dynamic_cast<LLDBLink*>(CMGetLink());
	if (link == NULL)
		{
		return;
		}

	lldb::SBTarget t = link->GetDebugger()->GetSelectedTarget();
	if (!t.IsValid())
		{
		return;
		}

	(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(kJFalse);

	JPtrArray<CMBreakpoint> bpList(JPtrArrayT::kForgetAll);	// ownership taken by CMBreakpointManager
	bpList.SetCompareFunction(CMBreakpointManager::CompareBreakpointLocations);
	bpList.SetSortOrder(JOrderedSetT::kSortAscending);

	JPtrArray<CMBreakpoint> otherList(JPtrArrayT::kForgetAll);	// ownership taken by CMBreakpointManager

	JSize count = t.GetNumBreakpoints();
	JString func, addr, cond;
	for (JIndex i=0; i<count; i++)
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

		if (b.GetCondition() != NULL)
			{
			cond = b.GetCondition();
			}
		else
			{
			cond.Clear();
			}

		CMBreakpoint* bp = NULL;
		if (file.IsValid())
			{
			const JString fullName = JCombinePathAndName(file.GetDirectory(), file.GetFilename());
			bp = jnew CMBreakpoint(b.GetID(), fullName, e.GetLine(), func, addr,
								  JI2B(b.IsEnabled()), action,
								  cond, b.GetIgnoreCount());
			assert( bp != NULL );
			}
		else if (fn.IsValid())
			{
			CMLocation loc;
			bp = jnew CMBreakpoint(b.GetID(), loc, func, addr,
								  JI2B(b.IsEnabled()), action,
								  cond, b.GetIgnoreCount());
			assert( bp != NULL );
			}

		if (bp != NULL)
			{
			bpList.InsertSorted(bp);

			// may be deleted or other status change

			if (action != CMBreakpoint::kKeepBreakpoint || b.GetIgnoreCount() > 0)
				{
				(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(kJTrue);
				}
			}
		}

	count = t.GetNumWatchpoints();
	if (count > 0)	// may be deleted when go out of scope
		{
		(CMGetLink()->GetBreakpointManager())->SetUpdateWhenStop(kJTrue);
		}

	for (JIndex i=0; i<count; i++)
		{
		lldb::SBWatchpoint w = t.GetWatchpointAtIndex(i);

		CMLocation loc;
		addr = JString(w.GetWatchAddress(), JString::kBase16);

		if (w.GetCondition() != NULL)
			{
			cond = w.GetCondition();
			}
		else
			{
			cond.Clear();
			}

		CMBreakpoint* bp = jnew CMBreakpoint(w.GetID(), loc, "", addr,
											JI2B(w.IsEnabled()), CMBreakpoint::kKeepBreakpoint,
											cond, w.GetIgnoreCount());
		assert( bp != NULL );
		bpList.InsertSorted(bp);
		}

	(CMGetLink()->GetBreakpointManager())->UpdateBreakpoints(bpList, otherList);
}
