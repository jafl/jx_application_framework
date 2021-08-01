/******************************************************************************
 CMBreakpointManager.cpp

	BASE CLASS = virtual public JBroadcaster

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "CMBreakpointManager.h"
#include "CMGetBreakpoints.h"
#include "cmGlobals.h"
#include "cmFileVersions.h"
#include <JListUtil.h>
#include <jDirUtil.h>
#include <jFileUtil.h>
#include <jAssert.h>

// JBroadcaster message types

const JUtf8Byte* CMBreakpointManager::kBreakpointsChanged =
	"BreakpointsChanged::CMBreakpointManager";

/******************************************************************************
 Constructor

 *****************************************************************************/

CMBreakpointManager::CMBreakpointManager
	(
	CMLink*				link,
	CMGetBreakpoints*	cmd
	)
	:
	itsLink(link),
	itsCmd(cmd),
	itsSavedBPList(nullptr),
	itsRestoreBreakpointsFlag(false),
	itsUpdateWhenStopFlag(false)
{
	assert( itsCmd != nullptr );

	itsBPList = jnew JPtrArray<CMBreakpoint>(JPtrArrayT::kDeleteAll);
	assert(itsBPList != nullptr);
	itsBPList->SetCompareFunction(CompareBreakpointLocations);
	itsBPList->SetSortOrder(JListT::kSortAscending);

	itsOtherList = jnew JPtrArray<CMBreakpoint>(JPtrArrayT::kDeleteAll);
	assert(itsOtherList != nullptr);

	ListenTo(itsLink);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

CMBreakpointManager::~CMBreakpointManager()
{
	jdelete itsCmd;
	jdelete itsBPList;
	jdelete itsSavedBPList;
	jdelete itsOtherList;
}

/******************************************************************************
 HasBreakpointAt

 *****************************************************************************/

bool
CMBreakpointManager::HasBreakpointAt
	(
	const CMLocation& loc
	)
	const
{
	CMBreakpoint target(loc.GetFileName(), loc.GetLineNumber());
	JIndex i;
	return itsBPList->SearchSorted(&target, JListT::kAnyMatch, &i);
}

/******************************************************************************
 GetBreakpoints

	*** CMBreakpointManager owns the objects returned in list!

 *****************************************************************************/

bool
CMBreakpointManager::GetBreakpoints
	(
	const JString&				fileName,
	JPtrArray<CMBreakpoint>*	list
	)
	const
{
	list->RemoveAll();
	list->SetCleanUpAction(JPtrArrayT::kForgetAll);

	if (JIsAbsolutePath(fileName) && JFileExists(fileName))
		{
		CMBreakpoint target(fileName, 1);
		bool found;
		const JIndex startIndex =
			itsBPList->SearchSorted1(&target, JListT::kFirstMatch, &found);

		const JSize count = itsBPList->GetElementCount();
		for (JIndex i=startIndex; i<=count; i++)
			{
			CMBreakpoint* bp = itsBPList->GetElement(i);
			if (bp->GetFileID() == target.GetFileID())
				{
				list->Append(bp);
				}
			else
				{
				break;
				}
			}
		}

	return !list->IsEmpty();
}

/******************************************************************************
 GetBreakpoints

	*** CMBreakpointManager owns the objects returned in list!

 *****************************************************************************/

bool
CMBreakpointManager::GetBreakpoints
	(
	const CMLocation&			loc,
	JPtrArray<CMBreakpoint>*	list
	)
	const
{
	list->RemoveAll();
	list->SetCleanUpAction(JPtrArrayT::kForgetAll);

	if (loc.GetFileID().IsValid())
		{
		CMBreakpoint target(loc.GetFileName(), loc.GetLineNumber());
		bool found;
		const JIndex startIndex =
			itsBPList->SearchSorted1(&target, JListT::kFirstMatch, &found);

		const JSize count = itsBPList->GetElementCount();
		for (JIndex i=startIndex; i<=count; i++)
			{
			CMBreakpoint* bp = itsBPList->GetElement(i);
			if (bp->GetLocation() == loc)
				{
				list->Append(bp);
				}
			else
				{
				break;
				}
			}
		}
	else if (!loc.GetFunctionName().IsEmpty())
		{
		const JString fn = loc.GetFunctionName() + "(";

		const JSize count = itsBPList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			CMBreakpoint* bp = itsBPList->GetElement(i);
			if (bp->GetFunctionName() == loc.GetFunctionName() ||
				bp->GetFunctionName().BeginsWith(fn))
				{
				list->Append(bp);
				}
			}
		}

	return !list->IsEmpty();
}

/******************************************************************************
 EnableAll

 ******************************************************************************/

void
CMBreakpointManager::EnableAll()
{
	for (auto* bp : *itsBPList)
		{
		if (!bp->IsEnabled())
			{
			itsLink->SetBreakpointEnabled(bp->GetDebuggerIndex(), true);
			}
		}
}

/******************************************************************************
 DisableAll

 ******************************************************************************/

void
CMBreakpointManager::DisableAll()
{
	for (auto* bp : *itsBPList)
		{
		if (bp->IsEnabled())
			{
			itsLink->SetBreakpointEnabled(bp->GetDebuggerIndex(), false);
			}
		}
}

/******************************************************************************
 ReadSetup

 ******************************************************************************/

void
CMBreakpointManager::ReadSetup
	(
	std::istream&		input,
	const JFileVersion	vers
	)
{
	itsLink->RemoveAllBreakpoints();

	JSize count;
	input >> count;

	if (count > 0)
		{
		jdelete itsSavedBPList;
		itsSavedBPList = jnew JPtrArray<CMBreakpoint>(JPtrArrayT::kDeleteAll);
		assert( itsSavedBPList != nullptr );
		}

	JString fileName, condition, commands;
	JIndex lineNumber;
	bool enabled, hasCondition, hasCommands;
	JSize ignoreCount;
	CMBreakpoint::Action action;
	long tempAction;
	for (JIndex i=1; i<=count; i++)
		{
		input >> fileName >> lineNumber;
		input >> JBoolFromString(enabled) >> tempAction >> ignoreCount;
		input >> JBoolFromString(hasCondition) >> condition;

		if (vers == 1)
			{
			input >> JBoolFromString(hasCommands) >> commands;
			}

		action = (CMBreakpoint::Action) tempAction;
		if (!hasCondition)
			{
			condition.Clear();
			}

		auto* bp = jnew CMBreakpoint(0, fileName, lineNumber,
											JString::empty, JString::empty,
											enabled, action, condition,
											ignoreCount);
		assert( bp != nullptr );
		itsSavedBPList->Append(bp);

		// set breakpoint after saving, so name resolution happens first

		itsLink->SetBreakpoint(fileName, lineNumber, action == CMBreakpoint::kRemoveBreakpoint);
		}
}

/******************************************************************************
 WriteSetup

 ******************************************************************************/

void
CMBreakpointManager::WriteSetup
	(
	std::ostream& output
	)
	const
{
	const JSize count = itsBPList->GetElementCount();
	output << ' ' << count;

	JString s;
	for (JIndex i=1; i<=count; i++)
		{
		CMBreakpoint* bp = itsBPList->GetElement(i);
		output << ' ' << bp->GetFileName();
		output << ' ' << bp->GetLineNumber();
		output << ' ' << JBoolToString(bp->IsEnabled());
		output << ' ' << (long) bp->GetAction();
		output << ' ' << bp->GetIgnoreCount();
		output << ' ' << JBoolToString(bp->GetCondition(&s));
		output << ' ' << s;
		}
}

/******************************************************************************
 Receive (virtual protected)

	We restore the breakpoints if the debugger is restarted.

 ******************************************************************************/

void
CMBreakpointManager::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(CMLink::kDebuggerRestarted))
		{
		itsRestoreBreakpointsFlag = true;
		}
	else if (sender == itsLink && message.Is(CMLink::kSymbolsLoaded))
		{
		const auto* info =
			dynamic_cast<const CMLink::SymbolsLoaded*>(&message);
		assert( info != nullptr );
		const JSize count = itsBPList->GetElementCount();
		if (info->Successful() && itsRestoreBreakpointsFlag && count > 0)
			{
			jdelete itsSavedBPList;
			itsSavedBPList = jnew JPtrArray<CMBreakpoint>(JPtrArrayT::kDeleteAll);
			assert( itsSavedBPList != nullptr );
			itsSavedBPList->CopyObjects(*itsBPList, JPtrArrayT::kDeleteAll, false);

			for (JIndex i=1; i<=count; i++)
				{
				itsLink->SetBreakpoint(*(itsBPList->GetElement(i)));
				}
			}
		itsRestoreBreakpointsFlag = false;
		}

	else if (sender == itsLink && message.Is(CMLink::kBreakpointsChanged))
		{
		itsCmd->Send();
		}
	else if (sender == itsLink && message.Is(CMLink::kProgramStopped))
		{
		if (itsUpdateWhenStopFlag)
			{
			itsCmd->Send();
			}
		}

	else
		{
		JBroadcaster::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateBreakpoints

 *****************************************************************************/

void
CMBreakpointManager::UpdateBreakpoints
	(
	const JPtrArray<CMBreakpoint>& bpList,
	const JPtrArray<CMBreakpoint>& otherList
	)
{
	itsBPList->CopyPointers(bpList, JPtrArrayT::kDeleteAll, false);
	itsOtherList->CopyPointers(otherList, JPtrArrayT::kDeleteAll, false);
	Broadcast(BreakpointsChanged());

	if (itsSavedBPList != nullptr &&
		itsSavedBPList->GetElementCount() == itsBPList->GetElementCount())
		{
		const JSize count = itsSavedBPList->GetElementCount();
		JString condition;
		for (JIndex i=1; i<=count; i++)
			{
			CMBreakpoint* bp = itsSavedBPList->GetElement(i);
			const JIndex j   = (itsBPList->GetElement(i))->GetDebuggerIndex();

			if (!bp->IsEnabled())
				{
				itsLink->SetBreakpointEnabled(j, false);
				}
			else if (bp->GetAction() == CMBreakpoint::kDisableBreakpoint)
				{
				itsLink->SetBreakpointEnabled(j, true, true);
				}

			if (bp->GetCondition(&condition))
				{
				itsLink->SetBreakpointCondition(j, condition);
				}

			if (bp->GetIgnoreCount() > 0)
				{
				itsLink->SetBreakpointIgnoreCount(j, bp->GetIgnoreCount());
				}
			}

		jdelete itsSavedBPList;
		itsSavedBPList = nullptr;
		}
}

/******************************************************************************
 BreakpointFileNameResolved

 *****************************************************************************/

void
CMBreakpointManager::BreakpointFileNameResolved
	(
	CMBreakpoint* bp
	)
{
	if (itsBPList->Includes(bp))
		{
		itsBPList->Remove(bp);
		itsBPList->InsertSorted(bp);
		Broadcast(BreakpointsChanged());
		}
}

/******************************************************************************
 BreakpointFileNameInvalid

 *****************************************************************************/

void
CMBreakpointManager::BreakpointFileNameInvalid
	(
	CMBreakpoint* bp
	)
{
	if (itsSavedBPList != nullptr)
		{
		itsSavedBPList->Remove(bp);
		}
}

/******************************************************************************
 CompareBreakpointLocations (static)

 ******************************************************************************/

JListT::CompareResult
CMBreakpointManager::CompareBreakpointLocations
	(
	CMBreakpoint* const & bp1,
	CMBreakpoint* const & bp2
	)
{
	JListT::CompareResult r = JFileID::Compare(bp1->GetFileID(), bp2->GetFileID());
	if (r == JListT::kFirstEqualSecond)
		{
		r = JCompareIndices(bp1->GetLineNumber(), bp2->GetLineNumber());
		}
	return r;
}
