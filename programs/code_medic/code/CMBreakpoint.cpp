/******************************************************************************
 CMBreakpoint.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright (C) 2001 by John Lindal.

 *****************************************************************************/

#include "CMBreakpoint.h"
#include "CMBreakpointManager.h"
#include "CMGetFullPath.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

CMBreakpoint::CMBreakpoint
	(
	const JSize			debuggerIndex,
	const CMLocation&	location,
	const JString&		fn,
	const JString&		addr,
	const bool		enabled,
	const Action		action,
	const JString&		condition,
	const JSize			ignoreCount
	)
	:
	itsDebuggerIndex(debuggerIndex),
	itsLocation(location),
	itsFnName(fn),
	itsAddr(addr),
	itsEnabledFlag(enabled),
	itsAction(action),
	itsCondition(condition),
	itsIgnoreCount(ignoreCount)
{
	CMBreakpointX();
}

CMBreakpoint::CMBreakpoint
	(
	const JSize		debuggerIndex,
	const JString&	fileName,
	const JSize		lineIndex,
	const JString&	fn,
	const JString&	addr,
	const bool	enabled,
	const Action	action,
	const JString&	condition,
	const JSize		ignoreCount
	)
	:
	itsDebuggerIndex(debuggerIndex),
	itsLocation(fileName, lineIndex),
	itsFnName(fn),
	itsAddr(addr),
	itsEnabledFlag(enabled),
	itsAction(action),
	itsCondition(condition),
	itsIgnoreCount(ignoreCount)
{
	CMBreakpointX();
}

// search target

CMBreakpoint::CMBreakpoint
	(
	const JString&	fileName,
	const JIndex	lineIndex
	)
	:
	itsDebuggerIndex(0),
	itsLocation(fileName, lineIndex),
	itsEnabledFlag(true),
	itsAction(kKeepBreakpoint),
	itsIgnoreCount(0)
{
	CMBreakpointX();
}

CMBreakpoint::CMBreakpoint
	(
	const JString& addr
	)
	:
	itsDebuggerIndex(0),
	itsAddr(addr),
	itsEnabledFlag(true),
	itsAction(kKeepBreakpoint),
	itsIgnoreCount(0)
{
	CMBreakpointX();
}

// private

void
CMBreakpoint::CMBreakpointX()
{
	const JString& fileName = GetFileName();
	if (!fileName.IsEmpty())	// search target may have empty file name
		{
		bool exists;
		JString fullName;
		if (CMGetLink()->FindFile(fileName, &exists, &fullName))
			{
			if (exists)
				{
				itsLocation.SetFileName(fullName);
				}
			}
		else
			{
			CMGetFullPath* cmd = CMGetLink()->CreateGetFullPath(fileName);
			ListenTo(cmd);
			}
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CMBreakpoint::~CMBreakpoint()
{
}

/******************************************************************************
 DisplayStatus

 *****************************************************************************/

void
CMBreakpoint::DisplayStatus()
	const
{
	CMGetLink()->ShowBreakpointInfo(itsDebuggerIndex);
}

/******************************************************************************
 ToggleEnabled

 *****************************************************************************/

void
CMBreakpoint::ToggleEnabled()
{
	CMGetLink()->SetBreakpointEnabled(itsDebuggerIndex, !itsEnabledFlag);
}

/******************************************************************************
 RemoveCondition

 *****************************************************************************/

void
CMBreakpoint::RemoveCondition()
{
	CMGetLink()->RemoveBreakpointCondition(itsDebuggerIndex);
}

/******************************************************************************
 SetIgnoreCount

 *****************************************************************************/

void
CMBreakpoint::SetIgnoreCount
	(
	const JSize count
	)
{
	CMGetLink()->SetBreakpointIgnoreCount(itsDebuggerIndex, count);
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
CMBreakpoint::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (message.Is(CMGetFullPath::kFileFound))
		{
		const auto* info =
			dynamic_cast<const CMGetFullPath::FileFound*>(&message);
		assert( info != nullptr );
		itsLocation.SetFileName(info->GetFullName());
		(CMGetLink()->GetBreakpointManager())->BreakpointFileNameResolved(this);
		}
	else if (message.Is(CMGetFullPath::kFileNotFound))
		{
		(CMGetLink()->GetBreakpointManager())->BreakpointFileNameInvalid(this);
		}
	else if (message.Is(CMGetFullPath::kNewCommand))
		{
		const auto* info =
			dynamic_cast<const CMGetFullPath::NewCommand*>(&message);
		assert( info != nullptr );
		ListenTo(info->GetNewCommand());
		}
	else
		{
		JBroadcaster::Receive(sender, message);
		}
}
